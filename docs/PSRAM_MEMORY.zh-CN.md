# PSRAM 内存策略

[English](PSRAM_MEMORY.md) | [简体中文](PSRAM_MEMORY.zh-CN.md)

平台数据和工作任务栈在访问方式、生命周期允许时使用 PSRAM，为硬件 DMA、内核对象和
Flash 操作保留内部 RAM。策略作用于应用与平台层；agent-studio-device-firmware、AOSL、RTSA
的不可变快照不变。
构建和真机结果统一记录在[验证矩阵](VALIDATION.md)。

## 配置与分配

隐藏配置 `CONFIG_MYBOT_PSRAM_ALLOCATIONS` 要求 PSRAM 已初始化并可通过堆分配，自动选择
`CONFIG_FREERTOS_TASK_CREATE_ALLOW_EXT_MEM` 与 `CONFIG_NVS_ALLOCATE_CACHE_IN_SPIRAM`。
平台 CMake 会拒绝未启用策略或外部任务栈支持的配置。更新后应重新配置已有构建目录。

迁移的数据和栈分配明确使用 `MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT`。分配失败时按既有初始化/清理路径
返回错误，避免占用内部 DMA 余量；PCM FIFO 也不再回退到内部 RAM。驱动自身的分配仍由
ESP-IDF 驱动管理能力要求与错误处理。

ESP32-S3 target defaults 已设置 `CONFIG_MBEDTLS_EXTERNAL_MEM_ALLOC=y`、2,048 字节的普通
malloc 内部内存优先阈值，以及 98,304 字节的内部保留池。TLS 工作区此前已使用 PSRAM，
迁移小型 HTTPS 连接包装对象不能再算作迁移整个 TLS 工作区的收益。保留池服务显式内部/
DMA 分配，不能保证存在一整块连续空间。

NVS 使用 ESP-IDF 官方的 PSRAM 优先分配器，页缓存和 key hash list 在 PSRAM 不足时仍可
回退到内部 RAM。收益取决于实际初始化的分区大小和条目：常见 8/16 MiB 布局仅有 16 KiB
NVS，SenseCAP 的普通 NVS 分区为 840 KiB；独立的 200 KiB 出厂分区未由本应用初始化。
不能用整颗 Flash 容量估算 RAM 收益。ESP-IDF 文档说明整数 NVS 操作约慢 2.5 倍，需实测
持久化和配网。

## 分配清单

下表是分配容量，不是已测得的内部堆降幅。部分缓冲此前已在 PSRAM，普通 malloc 原本也
可能选择任一区域。不同板型条目不累加；TCB 和分配器开销另计。

| 分配 | PSRAM 容量与生命周期 |
| --- | --- |
| 所有显示适配器的 LVGL 栈 | 显示 port 有效期间 7,168 字节；CoreS3 此前已如此配置 |
| CoreS3 像素视图 | `sizeof(View)`，包含两组形状数据和被标签保留的字符串；LVGL 控件删除后释放 |
| PCM 播放任务 | 每个播放上下文拥有 4,096 字节栈、7,680 字节 FIFO 和独立 480 字节 PCM 暂存区 |
| Codec 流数据 | 流上下文和暂存区在 PSRAM：CoreS3 采集 1,920 字节；ES8311/AMOLED175/VoCat/SenseCAP 每流 3,840 字节；XVF3800 每流 7,680 字节 |
| 原始 I2S 转换暂存区 | 根据板级槽配置，每个上下文 3,840 或 7,680 字节；包含原子状态的控制上下文仍在内部 RAM |
| ES8374 codec 控制对象 | codec 实例有效期间占用 `sizeof(audio_codec_es8374_t)` |
| CoreS3 离线播放对照测试 | 1,920 字节播放帧和可选的 1,920 字节采集帧；解码后的源 PCM 也在 PSRAM |
| CoreS3 JPEG 任务 | worker 存在期间使用 6,144 字节栈；已有的两张 153,600 字节采集缓冲和 131,072 字节 JPEG 输出也在 PSRAM |
| FT6336、AMOLED175、VoCat、SenseCAP 轮询任务 | 所选板型的输入任务栈为 3,072 字节 |
| 共享 CST816/FT5x06/GT911/CST226 输入和 XVF3800 按键任务 | 所选板型的输入任务栈为 4,096 字节 |
| 可选资源监控 | 启用 `CONFIG_MYBOT_DEBUG_RESOURCE_MONITOR=y` 时使用 3,072 字节栈 |
| 提示音与 HTTPS 包装对象 | 小型应用上下文显式使用 PSRAM；Ogg 包、解码 PCM 和 TLS 工作区此前已在 PSRAM |

所有显示板型另有 `宽度 × 16 × 2` 字节的内部 RGB565 DMA 条带，CoreS3 为 10,240 字节。
CoreS3 摄像头在当前 8,192 字节配置上限下还需要 7,680 字节连续内部 DVP DMA 缓冲，空闲
PSRAM 无法满足这些请求。内核 TCB、静态信号量、锁、ISR 数据和 PCM 缓冲的同步控制结构
仍留在内部 RAM。16 KiB 主任务栈也留在内部 RAM，因为启动和配网会调用关闭 cache 的
NVS/Flash 操作。第三方采集任务保持自身的分配策略。

## 任务所有权与清理

PSRAM 栈通过 ESP-IDF `xTaskCreateWithCaps` 或 LVGL port 的 `task_stack_caps` 创建，TCB
仍在内部 RAM。可停止的应用任务结束 I/O 和回调后，发布完成状态再挂起；owner 保留 handle，
确认任务已挂起后调用 `vTaskDeleteWithCaps`，再释放相关资源。这样不需要为任务自删除创建
内部清理线程。完成状态在超时后仍可读取，后续可重试清理。

PCM、视频和可停止输入的清理在等待任务或回调超时时保留资源；再次启动前必须由重试完成
清理。进程生命周期的输入和监控任务保留 PSRAM 栈直到重启。LVGL 工作任务仍由其 port
负责清理。迁移会写 Flash 的任务栈之前，必须审查完整调用链。

## 验证命令与真机回归

加载 ESP-IDF 5.5.2 后，可用下列命令创建 CoreS3 开发构建：

```sh
idf.py -B build/cores3-psram \
  -DMYBOT_BOARD=m5stack-core-s3 \
  -DSDKCONFIG=build/cores3-psram/sdkconfig \
  -DSDKCONFIG_DEFAULTS="sdkconfig.defaults;ci/ptime60.defaults;ci/video.defaults" build
```

已有目录使用 `idf.py -B <build-dir> reconfigure build`。检查生成配置中的策略、外部任务栈
和 NVS cache 符号；带屏 profile 还必须保留 `CONFIG_MYBOT_LVGL_UI=y`。以下宿主检查覆盖
分配失败、部分清理、超时、回调顺序与重复启停：

```sh
sh tests/audio_video_psram/run.sh
AUDIO_VIDEO_SANITIZERS=1 sh tests/audio_video_psram/run.sh
sh tests/cst816s_input/run.sh
sh tests/cst226_input/run.sh
sh tests/st7789_lifecycle/run.sh
sh tests/cores3_pixel_ui/run.sh
git diff --check
```

每个受影响设备需验证配网/重连、HTTPS 配对、NVS 音量保存与重启恢复、重复 SDK/音视频启停、
回调结束顺序、触摸/长按、显示方向与刷新、有限提示音排空，以及全双工音频与视频/UI 并行。
记录每核 CPU、内部/PSRAM 最小空闲堆、stream-on 前和多轮启停中的最大 DMA 连续块。
采集节奏、服务端接收要与本地提交分开检查。新的物理测试结果写入
[VALIDATION.md](VALIDATION.md)；构建和宿主测试不能证明 cache 安全、音频连续性或硬件行为。
