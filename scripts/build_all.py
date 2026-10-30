#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0
"""Build firmware for all supported mybot ESP32-S3 board profiles.

Display capability and video capability are derived from each board profile:
display boards automatically select the LVGL Kconfig, and a board that adds
the CoreS3 camera source receives the video defaults automatically.

Build directories are kept under the repository's ``build/`` directory and
merged firmware images are written to ``releases/`` by default.
"""

from __future__ import annotations

import argparse
import json
import os
import re
import shutil
import stat
import subprocess
import sys
import time
from dataclasses import dataclass
from pathlib import Path


PROJECT_ROOT = Path(__file__).resolve().parent.parent
BOARDS_CMAKE = PROJECT_ROOT / "components/mybot_platform/boards/boards.cmake"
RELEASES_DIR = PROJECT_ROOT / "releases"
GENERATED_ROOT_MARKER = ".mybot-generated-root.json"


def format_elapsed(seconds: float) -> str:
    total = max(0, int(seconds))
    hours, remainder = divmod(total, 3600)
    minutes, seconds = divmod(remainder, 60)
    return f"{hours:02d}:{minutes:02d}:{seconds:02d}"


@dataclass(frozen=True)
class BoardProfile:
    name: str
    has_display: bool
    video_capable: bool


def supported_boards() -> list[BoardProfile]:
    text = BOARDS_CMAKE.read_text(encoding="utf-8")
    names = re.findall(r'^\s+"([^"\n]+)"\s*$', text, re.MULTILINE)
    profiles: list[BoardProfile] = []
    for name in names:
        profile_path = PROJECT_ROOT / "components/mybot_platform/boards" / name / "board.cmake"
        if not profile_path.is_file():
            raise RuntimeError(f"board profile is missing: {profile_path}")
        profile = profile_path.read_text(encoding="utf-8")
        profiles.append(
            BoardProfile(
                name=name,
                has_display=(
                    "mybot_display_add_" in profile
                    or "set(MYBOT_GENERIC_DISPLAY " in profile
                ),
                video_capable="cores3_camera_video.c" in profile,
            )
        )
    if not profiles:
        raise RuntimeError(f"no boards found in {BOARDS_CMAKE}")
    return profiles


def idf_command() -> str:
    idf_path = os.environ.get("IDF_PATH")
    if not idf_path:
        raise RuntimeError("ESP-IDF is not sourced; run source /path/to/esp-idf/export.sh first")
    idf = shutil.which("idf.py")
    if not idf:
        raise RuntimeError("idf.py is not in PATH; source the ESP-IDF export script first")
    version = subprocess.run([idf, "--version"], check=True, capture_output=True, text=True).stdout.strip()
    if version != "ESP-IDF v6.1":
        raise RuntimeError(f"expected ESP-IDF v6.1, got {version}")
    return idf


def run(idf: str, args: list[str], dry_run: bool) -> None:
    command = [idf, *args]
    print(">>>", " ".join(command))
    if dry_run:
        return
    subprocess.run(command, cwd=PROJECT_ROOT, check=True)


def normalize_generated_root(path: Path, label: str) -> Path:
    """Reject unsafe paths before normalization can hide links or parent traversal."""
    path = path.expanduser()
    if ".." in path.parts:
        raise RuntimeError(f"refusing parent traversal in {label} path: {path}")
    path = Path(os.path.abspath(path))
    if path.anchor != "/":
        raise RuntimeError(f"refusing non-canonical {label} path: {path}")
    for component in (path, *path.parents):
        if component.is_symlink():
            raise RuntimeError(f"refusing symlink in {label} path: {component}")
    protected = {Path(path.anchor), Path.home().resolve(), PROJECT_ROOT, *PROJECT_ROOT.parents}
    if path in protected:
        raise RuntimeError(f"refusing unsafe {label} path: {path}")
    if PROJECT_ROOT in path.parents and not any(
        path == allowed or allowed in path.parents
        for allowed in (PROJECT_ROOT / "build", RELEASES_DIR)
    ):
        raise RuntimeError(f"{label} path must not be inside the project source tree: {path}")
    return path


def _open_directory(path: Path, *, create: bool = False) -> int:
    """Walk from the filesystem root without following any symlink component."""
    if not hasattr(os, "O_NOFOLLOW") or not hasattr(os, "O_DIRECTORY"):
        raise RuntimeError("safe output directory handling requires POSIX directory descriptors")
    flags = os.O_RDONLY | os.O_DIRECTORY | os.O_NOFOLLOW
    directory = os.open(path.anchor, flags)
    try:
        for component in path.parts[1:]:
            try:
                child = os.open(component, flags, dir_fd=directory)
            except FileNotFoundError:
                if not create:
                    raise
                os.mkdir(component, dir_fd=directory)
                child = os.open(component, flags, dir_fd=directory)
            os.close(directory)
            directory = child
        return directory
    except BaseException:
        os.close(directory)
        raise


def _root_identity(path: Path, label: str) -> dict[str, str | int]:
    return {"format": 1, "project": str(PROJECT_ROOT), "root": str(path), "role": label}


def _validate_root_contents(directory: int, path: Path, label: str) -> bool:
    """An existing nonempty root must already carry this script's ownership marker."""
    if not os.listdir(directory):
        return False
    try:
        marker = os.open(
            GENERATED_ROOT_MARKER, os.O_RDONLY | os.O_NOFOLLOW | os.O_NONBLOCK, dir_fd=directory
        )
        with os.fdopen(marker, "r", encoding="utf-8") as stream:
            info = os.fstat(stream.fileno())
            if not stat.S_ISREG(info.st_mode) or info.st_size > 4096:
                raise ValueError("marker is not a small regular file")
            identity = json.loads(stream.read(4097))
        if (
            not isinstance(identity, dict)
            or type(identity.get("format")) is not int
            or identity != _root_identity(path, label)
        ):
            raise ValueError("marker does not match this project, directory and role")
    except (OSError, ValueError) as error:
        raise RuntimeError(
            f"refusing unowned or invalid {label} directory: {path}; "
            "choose a new/empty dedicated directory, or move the old directory aside "
            "after inspecting its contents"
        ) from error
    return True


def validate_generated_root(path: Path, label: str) -> Path:
    path = normalize_generated_root(path, label)
    try:
        directory = _open_directory(path)
    except FileNotFoundError:
        return path
    except OSError as error:
        raise RuntimeError(f"refusing invalid {label} directory: {path}") from error
    try:
        _validate_root_contents(directory, path, label)
    finally:
        os.close(directory)
    return path


def validate_output_roots(build_root: Path, output_root: Path) -> tuple[Path, Path]:
    """Preflight both roots before either one can be created or cleaned."""
    build_root = validate_generated_root(build_root, "build")
    output_root = validate_generated_root(output_root, "output")
    if (
        build_root == output_root
        or build_root in output_root.parents
        or output_root in build_root.parents
    ):
        raise RuntimeError("build and output roots must be separate, non-overlapping directories")
    return build_root, output_root


def prepare_generated_root(path: Path, label: str, dry_run: bool, *, clean: bool = True) -> None:
    """Only remove children of an owned root, using its descriptor as the deletion anchor."""
    path = validate_generated_root(path, label)
    print(f"{'Cleaning' if clean else 'Preparing'} {label}: {path}")
    if dry_run:
        return
    if clean and (sys.version_info < (3, 11) or not shutil.rmtree.avoids_symlink_attacks):
        raise RuntimeError("safe cleanup requires Python 3.11+ with symlink-safe rmtree")
    directory = _open_directory(path, create=True)
    try:
        owned = _validate_root_contents(directory, path, label)
        if not owned:
            marker = os.open(
                GENERATED_ROOT_MARKER,
                os.O_WRONLY | os.O_CREAT | os.O_EXCL | os.O_NOFOLLOW,
                0o600,
                dir_fd=directory,
            )
            with os.fdopen(marker, "w", encoding="utf-8") as stream:
                json.dump(_root_identity(path, label), stream)
                stream.write("\n")
        if clean:
            for name in os.listdir(directory):
                if name == GENERATED_ROOT_MARKER:
                    continue
                info = os.stat(name, dir_fd=directory, follow_symlinks=False)
                if stat.S_ISDIR(info.st_mode):
                    shutil.rmtree(name, dir_fd=directory)
                else:
                    os.unlink(name, dir_fd=directory)
    finally:
        os.close(directory)


def variant_name(board: BoardProfile, language: str, video: bool) -> str:
    suffix = "-video" if video else ""
    return f"{board.name}-{language}{suffix}"


def build_variant(
    idf: str,
    board: BoardProfile,
    language: str,
    video: bool,
    build_root: Path,
    output_root: Path,
    dry_run: bool,
) -> Path:
    started_at = time.perf_counter()
    name = variant_name(board, language, video)
    build_dir = build_root / f"build-{name}"
    sdkconfig = build_dir / "sdkconfig"
    output_path = output_root / f"{name}.bin"
    defaults = ["sdkconfig.defaults", "ci/ptime60.defaults"]
    if language == "en-US":
        defaults.append("ci/en-us.defaults")
    if video:
        defaults.append("ci/video.defaults")
    defaults_value = ";".join(defaults)

    base_args = [
        "-B",
        str(build_dir),
        f"-DMYBOT_BOARD={board.name}",
        f"-DSDKCONFIG={sdkconfig}",
        f"-DSDKCONFIG_DEFAULTS={defaults_value}",
    ]
    print(f"\nBuilding {name} (display={'yes' if board.has_display else 'no'}, "
          f"video={'yes' if video else 'no'})")
    try:
        run(idf, [*base_args, "reconfigure"], dry_run)
        run(idf, [*base_args, "build"], dry_run)
        run(idf, [*base_args, "size"], dry_run)
        run(idf, [*base_args, "merge-bin", "-o", str(output_path)], dry_run)
    except Exception:
        print(f"[FAIL] {name} elapsed={format_elapsed(time.perf_counter() - started_at)}")
        raise
    print(f"[OK] {name} elapsed={format_elapsed(time.perf_counter() - started_at)}")
    return output_path


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--board",
        action="append",
        help="board profile to build; repeat for multiple boards (default: all)",
    )
    parser.add_argument(
        "--language",
        choices=("zh-CN", "en-US", "both"),
        default="both",
        help="firmware language (default: both)",
    )
    parser.add_argument(
        "--video",
        choices=("auto", "on", "off"),
        default="auto",
        help="video policy: auto enables it on capable boards (default: auto)",
    )
    parser.add_argument(
        "--build-root",
        type=Path,
        default=PROJECT_ROOT / "build",
        help="dedicated new/empty or script-owned build root (default: repository build/)",
    )
    parser.add_argument(
        "--output-root",
        type=Path,
        default=RELEASES_DIR,
        help="dedicated new/empty or script-owned firmware root (default: repository releases/)",
    )
    parser.add_argument("--dry-run", action="store_true", help="print commands without building")
    parser.add_argument(
        "--no-clean",
        action="store_true",
        help="keep generated outputs for an incremental build; path and ownership checks still apply",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    profiles = supported_boards()
    by_name = {profile.name: profile for profile in profiles}
    selected_names = args.board or [profile.name for profile in profiles]
    unknown = sorted(set(selected_names) - set(by_name))
    if unknown:
        raise RuntimeError(f"unsupported board(s): {', '.join(unknown)}")
    selected = [by_name[name] for name in selected_names]
    languages = ("zh-CN", "en-US") if args.language == "both" else (args.language,)

    if args.video == "on" and any(not board.video_capable for board in selected):
        invalid = [board.name for board in selected if not board.video_capable]
        raise RuntimeError(f"video is unsupported on: {', '.join(invalid)}")

    build_root, output_root = validate_output_roots(args.build_root, args.output_root)
    idf = idf_command()
    prepare_generated_root(build_root, "build", args.dry_run, clean=not args.no_clean)
    prepare_generated_root(output_root, "output", args.dry_run, clean=not args.no_clean)

    outputs: list[Path] = []
    batch_started_at = time.perf_counter()
    try:
        for board in selected:
            for language in languages:
                video = board.video_capable and args.video != "off"
                outputs.append(
                    build_variant(idf, board, language, video, build_root, output_root, args.dry_run)
                )
    finally:
        print(f"\nTotal elapsed: {format_elapsed(time.perf_counter() - batch_started_at)}")

    print(f"\nPlanned variants: {len(outputs)}")
    print(f"Output directory: {output_root}")
    for output in outputs:
        print(f"  {output}")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (OSError, RuntimeError, subprocess.CalledProcessError) as error:
        print(f"error: {error}", file=sys.stderr)
        raise SystemExit(1)
