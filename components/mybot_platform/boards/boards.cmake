set(MYBOT_SUPPORTED_BOARDS
    "atom-echos3r"
    "bread-compact-wifi-lcd"
    "bread-compact-wifi"
    "esp-vocat"
    "esp32-s3-touch-amoled-1.75"
    "esp32-s3-touch-amoled-1.75c"
    "esp32-s3-touch-amoled-2.16"
    "esp32-s3-touch-amoled-1.8-v2"
    "esp32-s3-touch-amoled-1.8"
    "esp32-s3-touch-amoled-1.32"
    "esp32-s3-touch-amoled-1.43c"
    "esp32-s3-touch-amoled-2.06"
    "esp32-s3-touch-lcd-1.85"
    "esp32-s3-touch-lcd-1.85b"
    "esp32-s3-touch-lcd-1.85c"
    "esp32-s3-touch-lcd-1.54"
    "esp32-s3-touch-lcd-1.83"
    "esp32-s3-lcd-0.85"
    "esp-box"
    "esp-box-3"
    "esp-box-lite"
    "esp32s3-korvo2-v3"
    "genjutech-s3-1.54tft"
    "atoms3r-echo-base"
    "atoms3r-echo-pyramid"
    "aipi-lite"
    "atk-dnesp32s3-box0"
    "df-k10"
    "doit-s3-aibox"
    "freenove-esp32s3-display-2.8-lcd"
    "hu-087"
    "lichuang-dev"
    "magiclick-2p4"
    "m5stack-core-s3"
    "m5stack-stick-s3"
    "m5stack-stopwatch"
    "mixgo-nova"
    "movecall-cuican-esp32s3"
    "movecall-moji-esp32s3"
    "nulllab-ai-vox-v3"
    "quandong-s3-dev"
    "respeaker-flex-xvf3800-circular4-xiao"
    "rymcu-bigsmart"
    "sensecap-watcher"
    "sp-esp32-s3-1.28-box"
    "sp-esp32-s3-1.54-muma"
    "taiji-pi-s3"
    "xingzhi-abs-2.0"
    "xingzhi-cube-0.85tft-wifi"
    "xingzhi-cube-0.96oled-wifi"
    "xingzhi-cube-1.54tft-wifi"
    "xingzhi-metal-1.54-wifi"
    "zhengchen-1.54tft-ml307"
    "zhengchen-1.54tft-wifi"
)
set(_MYBOT_BOARDS_ROOT "${CMAKE_CURRENT_LIST_DIR}")

function(mybot_resolve_board board)
    if(board STREQUAL "atom-echos3r")
        set(profile "${_MYBOT_BOARDS_ROOT}/atom-echos3r/board.cmake")
    elseif(board STREQUAL "bread-compact-wifi-lcd")
        set(profile "${_MYBOT_BOARDS_ROOT}/bread-compact-wifi-lcd/board.cmake")
    elseif(board STREQUAL "bread-compact-wifi")
        set(profile "${_MYBOT_BOARDS_ROOT}/bread-compact-wifi/board.cmake")
    elseif(board STREQUAL "esp-vocat")
        set(profile "${_MYBOT_BOARDS_ROOT}/esp-vocat/board.cmake")
    elseif(board STREQUAL "esp32-s3-touch-amoled-1.75")
        set(profile "${_MYBOT_BOARDS_ROOT}/esp32-s3-touch-amoled-1.75/board.cmake")
    elseif(board STREQUAL "esp32-s3-touch-amoled-1.75c")
        set(profile "${_MYBOT_BOARDS_ROOT}/esp32-s3-touch-amoled-1.75c/board.cmake")
    elseif(board STREQUAL "esp32-s3-touch-amoled-2.16")
        set(profile "${_MYBOT_BOARDS_ROOT}/esp32-s3-touch-amoled-2.16/board.cmake")
    elseif(board STREQUAL "esp32-s3-touch-amoled-1.8-v2")
        set(profile "${_MYBOT_BOARDS_ROOT}/esp32-s3-touch-amoled-1.8-v2/board.cmake")
    elseif(board STREQUAL "esp32-s3-touch-amoled-1.8")
        set(profile "${_MYBOT_BOARDS_ROOT}/esp32-s3-touch-amoled-1.8/board.cmake")
    elseif(board STREQUAL "esp32-s3-touch-amoled-1.32")
        set(profile "${_MYBOT_BOARDS_ROOT}/esp32-s3-touch-amoled-1.32/board.cmake")
    elseif(board STREQUAL "esp32-s3-touch-amoled-1.43c")
        set(profile "${_MYBOT_BOARDS_ROOT}/esp32-s3-touch-amoled-1.43c/board.cmake")
    elseif(board STREQUAL "esp32-s3-touch-amoled-2.06")
        set(profile "${_MYBOT_BOARDS_ROOT}/esp32-s3-touch-amoled-2.06/board.cmake")
    elseif(board STREQUAL "esp32-s3-touch-lcd-1.85")
        set(profile "${_MYBOT_BOARDS_ROOT}/esp32-s3-touch-lcd-1.85/board.cmake")
    elseif(board STREQUAL "esp32-s3-touch-lcd-1.85b")
        set(profile "${_MYBOT_BOARDS_ROOT}/esp32-s3-touch-lcd-1.85b/board.cmake")
    elseif(board STREQUAL "esp32-s3-touch-lcd-1.85c")
        set(profile "${_MYBOT_BOARDS_ROOT}/esp32-s3-touch-lcd-1.85c/board.cmake")
    elseif(board STREQUAL "esp32-s3-touch-lcd-1.54")
        set(profile "${_MYBOT_BOARDS_ROOT}/esp32-s3-touch-lcd-1.54/board.cmake")
    elseif(board STREQUAL "esp32-s3-touch-lcd-1.83")
        set(profile "${_MYBOT_BOARDS_ROOT}/esp32-s3-touch-lcd-1.83/board.cmake")
    elseif(board STREQUAL "esp32-s3-lcd-0.85")
        set(profile "${_MYBOT_BOARDS_ROOT}/esp32-s3-lcd-0.85/board.cmake")
    elseif(board STREQUAL "esp-box")
        set(profile "${_MYBOT_BOARDS_ROOT}/esp-box/board.cmake")
    elseif(board STREQUAL "esp-box-3")
        set(profile "${_MYBOT_BOARDS_ROOT}/esp-box-3/board.cmake")
    elseif(board STREQUAL "esp-box-lite")
        set(profile "${_MYBOT_BOARDS_ROOT}/esp-box-lite/board.cmake")
    elseif(board STREQUAL "esp32s3-korvo2-v3")
        set(profile "${_MYBOT_BOARDS_ROOT}/esp32s3-korvo2-v3/board.cmake")
    elseif(board STREQUAL "genjutech-s3-1.54tft")
        set(profile "${_MYBOT_BOARDS_ROOT}/genjutech-s3-1.54tft/board.cmake")
    elseif(board STREQUAL "atoms3r-echo-base")
        set(profile "${_MYBOT_BOARDS_ROOT}/atoms3r-echo-base/board.cmake")
    elseif(board STREQUAL "atoms3r-echo-pyramid")
        set(profile "${_MYBOT_BOARDS_ROOT}/atoms3r-echo-pyramid/board.cmake")
    elseif(board STREQUAL "aipi-lite")
        set(profile "${_MYBOT_BOARDS_ROOT}/aipi-lite/board.cmake")
    elseif(board STREQUAL "atk-dnesp32s3-box0")
        set(profile "${_MYBOT_BOARDS_ROOT}/atk-dnesp32s3-box0/board.cmake")
    elseif(board STREQUAL "df-k10")
        set(profile "${_MYBOT_BOARDS_ROOT}/df-k10/board.cmake")
    elseif(board STREQUAL "doit-s3-aibox")
        set(profile "${_MYBOT_BOARDS_ROOT}/doit-s3-aibox/board.cmake")
    elseif(board STREQUAL "hu-087")
        set(profile "${_MYBOT_BOARDS_ROOT}/hu-087/board.cmake")
    elseif(board STREQUAL "lichuang-dev")
        set(profile "${_MYBOT_BOARDS_ROOT}/lichuang-dev/board.cmake")
    elseif(board STREQUAL "magiclick-2p4")
        set(profile "${_MYBOT_BOARDS_ROOT}/magiclick-2p4/board.cmake")
    elseif(board STREQUAL "m5stack-core-s3")
        set(profile "${_MYBOT_BOARDS_ROOT}/m5stack-core-s3/board.cmake")
    elseif(board STREQUAL "m5stack-stick-s3")
        set(profile "${_MYBOT_BOARDS_ROOT}/m5stack-stick-s3/board.cmake")
    elseif(board STREQUAL "m5stack-stopwatch")
        set(profile "${_MYBOT_BOARDS_ROOT}/m5stack-stopwatch/board.cmake")
    elseif(board STREQUAL "mixgo-nova")
        set(profile "${_MYBOT_BOARDS_ROOT}/mixgo-nova/board.cmake")
    elseif(board STREQUAL "movecall-cuican-esp32s3")
        set(profile "${_MYBOT_BOARDS_ROOT}/movecall-cuican-esp32s3/board.cmake")
    elseif(board STREQUAL "movecall-moji-esp32s3")
        set(profile "${_MYBOT_BOARDS_ROOT}/movecall-moji-esp32s3/board.cmake")
    elseif(board STREQUAL "nulllab-ai-vox-v3")
        set(profile "${_MYBOT_BOARDS_ROOT}/nulllab-ai-vox-v3/board.cmake")
    elseif(board STREQUAL "quandong-s3-dev")
        set(profile "${_MYBOT_BOARDS_ROOT}/quandong-s3-dev/board.cmake")
    elseif(board STREQUAL "respeaker-flex-xvf3800-circular4-xiao")
        set(profile
            "${_MYBOT_BOARDS_ROOT}/respeaker-flex-xvf3800-circular4-xiao/board.cmake")
    elseif(board STREQUAL "rymcu-bigsmart")
        set(profile "${_MYBOT_BOARDS_ROOT}/rymcu-bigsmart/board.cmake")
    elseif(board STREQUAL "sensecap-watcher")
        set(profile "${_MYBOT_BOARDS_ROOT}/sensecap-watcher/board.cmake")
    elseif(board STREQUAL "sp-esp32-s3-1.28-box")
        set(profile "${_MYBOT_BOARDS_ROOT}/sp-esp32-s3-1.28-box/board.cmake")
    elseif(board STREQUAL "sp-esp32-s3-1.54-muma")
        set(profile "${_MYBOT_BOARDS_ROOT}/sp-esp32-s3-1.54-muma/board.cmake")
    elseif(board STREQUAL "taiji-pi-s3")
        set(profile "${_MYBOT_BOARDS_ROOT}/taiji-pi-s3/board.cmake")
    elseif(board STREQUAL "xingzhi-abs-2.0")
        set(profile "${_MYBOT_BOARDS_ROOT}/xingzhi-abs-2.0/board.cmake")
    elseif(board STREQUAL "xingzhi-cube-0.85tft-wifi")
        set(profile "${_MYBOT_BOARDS_ROOT}/xingzhi-cube-0.85tft-wifi/board.cmake")
    elseif(board STREQUAL "xingzhi-cube-0.96oled-wifi")
        set(profile "${_MYBOT_BOARDS_ROOT}/xingzhi-cube-0.96oled-wifi/board.cmake")
    elseif(board STREQUAL "xingzhi-cube-1.54tft-wifi")
        set(profile "${_MYBOT_BOARDS_ROOT}/xingzhi-cube-1.54tft-wifi/board.cmake")
    elseif(board STREQUAL "xingzhi-metal-1.54-wifi")
        set(profile "${_MYBOT_BOARDS_ROOT}/xingzhi-metal-1.54-wifi/board.cmake")
    elseif(board STREQUAL "zhengchen-1.54tft-ml307")
        set(profile "${_MYBOT_BOARDS_ROOT}/zhengchen-1.54tft-ml307/board.cmake")
    elseif(board STREQUAL "zhengchen-1.54tft-wifi")
        set(profile "${_MYBOT_BOARDS_ROOT}/zhengchen-1.54tft-wifi/board.cmake")
    elseif(board STREQUAL "freenove-esp32s3-display-2.8-lcd")
        set(profile "${_MYBOT_BOARDS_ROOT}/freenove-esp32s3-display-2.8-lcd/board.cmake")
    else()
        message(FATAL_ERROR "Unsupported MYBOT_BOARD: ${board}")
    endif()
    set(MYBOT_BOARD_PROFILE "${profile}" PARENT_SCOPE)
endfunction()
