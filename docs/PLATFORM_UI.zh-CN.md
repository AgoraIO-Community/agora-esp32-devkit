# 平台 LVGL UI

[English](PLATFORM_UI.md) | [简体中文](PLATFORM_UI.zh-CN.md)

所有带屏板型均使用LVGL；`m5stack-core-s3`选择工艺灰像素眼镜，其他带屏板型保留共享流程
视图。`MYBOT_BOARD`自动决定界面并设置隐藏的 `CONFIG_MYBOT_LVGL_UI`，不增加手工
渲染器开关或sdkconfig覆盖。SDK LCD接口、音频/摄像头和触摸/按键归属保留原板级行为。

## 当前适配器

| Board | 面板与逻辑尺寸 | LVGL 适配器 | 输入行为 |
| --- | --- | --- | --- |
| `zhengchen-1.54tft-ml307` | ST7789，240 × 240 | 通用 ST7789 | 原有按键 |
| `zhengchen-1.54tft-wifi` | ST7789，240 × 240 | 通用 ST7789 | 原有按键 |
| `xingzhi-cube-1.54tft-wifi` | ST7789，240 × 240 | 通用面板适配器 | Boot 与音量按键 |
| `bread-compact-wifi-lcd` | ST7789，240 × 320 | 通用面板适配器 | 仅 Boot |
| `m5stack-core-s3` | ILI9342，320 × 240 | CoreS3适配器、像素眼镜视图 | 原有 FT6336 手势 |
| `m5stack-stick-s3` | ST7789P3，135 × 240 | 通用 ST7789 | 原有主按键 |
| `esp-vocat` | ST77916，360 × 360 | 通用面板适配器 | 原有 CST816S/Boot 路径 |
| `esp32-s3-touch-amoled-1.75` | CO5300，466 × 466 | 通用面板适配器 | 原有触摸/按键路径 |
| `esp32-s3-touch-amoled-1.75c` | CO5300，466 × 466 | 通用面板适配器 | 原有触摸/按键路径 |
| `esp32-s3-touch-amoled-2.16` | CO5300 480 变体，480 × 480 | 通用面板适配器 | CST9217 与 Boot |
| `esp32-s3-touch-amoled-1.8-v2` | CO5300 368×448 变体，368 × 448 | 通用面板适配器 | CST816S 与 Boot |
| `esp32-s3-touch-lcd-1.85` | ST77916，360 × 360 | 通用面板适配器 | CST816S 与 Boot |
| `genjutech-s3-1.54tft` | ST7789，240 × 240 | 通用 ST7789 | Boot 与音量按键 |
| `zhengchen-cam` | ST7789，320 × 240 | 通用 ST7789 | Boot 与音量按键 |
| `labplus-ledong-v2`、`labplus-mpython-v3` | ST7789 兼容，320 × 172 | 通用 ST7789 | Boot |
| `atk-dnesp32s3` | ST7789，320 × 240 | 通用 ST7789 | Boot |
| `yunliao-s3` | ST7789，320 × 240 | 通用 ST7789 | Boot |
| `lilygo-t-cameraplus-s3`、`lilygo-t-cameraplus-s3-v1.2` | ST7789，240 × 240 | 通用 ST7789 | CST816D 与 Boot |
| `lilygo-t-circle-s3` | GC9D01N，160 × 160 圆屏 | 通用面板适配器、圆屏布局 | CST816D 与 Boot |
| `jiuchuan-s3` | GC9309NA，240 × 296 | 通用面板适配器 | Boot 与音量按键 |
| `sensecap-watcher` | SPD2010，412 × 412 | 通用面板适配器 | 原有旋钮路径 |
| `xingzhi-metal-1.54-wifi`、`xingzhi-abs-2.0` | ST7789，240 × 240 | 通用面板适配器 | Boot/触摸或 Boot/音量键 |
| `xingzhi-cube-0.85tft-wifi` | NV3023，128 × 128 | 通用面板适配器 | Boot |
| `xingzhi-cube-0.96oled-wifi` | 无屏 | 无 | Boot 与音量键 |
| `m5stack-stopwatch` | CO5300，466 × 466 圆屏 | 通用面板适配器 | CST820B 兼容触摸与按键 |
| `esp32-s3-touch-amoled-1.8` | SH8601，368 × 448 | 通用面板适配器 | FT5x06 与 Boot |
| `esp32-s3-touch-amoled-1.32`、`esp32-s3-touch-amoled-1.43c`、`esp32-s3-touch-amoled-2.06` | SH8601，466 × 466 或 410 × 502 | 通用面板适配器 | Boot；2.06 使用 FT5x06 |
| `atoms3r-echo-base`、`atoms3r-echo-pyramid` | GC9A01 类，128 × 128 | 通用面板适配器 | Boot |
| `sp-esp32-s3-1.28-box` | GC9A01，240 × 240 圆屏 | 通用面板适配器 | CST816S 兼容触摸与 Boot |
| `esp-box`、`esp-box-3`、`esp-box-lite` | ILI9341，320 × 240 | 通用面板适配器 | Boot；未实现触摸 |
| `esp32s3-korvo2-v3`、`df-k10` | ILI9341，320 × 240 | 通用面板适配器 | Boot |
| `lichuang-dev`、`rymcu-bigsmart` | ST7789，320 × 240 | 通用面板适配器 | FT5x06 或 GT911，以及 Boot |
| `quandong-s3-dev` | ILI9341，320 × 240 | 通用面板适配器 | Boot |
| `mixgo-nova` | ST7789，128 × 160 | 通用面板适配器 | Boot |
| `taiji-pi-s3` | ST77916，360 × 360 圆屏 | 通用面板适配器 | CST816S 与 Boot |
| `freenove-esp32s3-display-2.8-lcd` | ILI9341，320 × 240 | 通用面板适配器 | Boot |
| `esp32-s3-touch-lcd-1.54`、`esp32-s3-touch-lcd-1.83` | ST7789，240 × 240 或 240 × 284 | 通用 ST7789 | CST816S 与按键 |
| `esp32-s3-lcd-0.85`、`aipi-lite` | ST7789，128 × 128 | 通用 ST7789 紧凑布局 | 按键 |
| `esp32-s3-touch-lcd-1.85b`、`esp32-s3-touch-lcd-1.85c` | ST77916，360 × 360 | 通用面板适配器 | CST816S 兼容触摸与 Boot |
| `sp-esp32-s3-1.54-muma` | ST7789，240 × 240 | 通用 ST7789 | CST816S 与 Boot |
| `atk-dnesp32s3-box0`、`nulllab-ai-vox-v3` | ST7789，240 × 240 | 通用 ST7789 | 按键 |
| `magiclick-2p4` | NV3023，128 × 128 | 通用面板适配器 | 三个按键 |
| `movecall-cuican-esp32s3`、`movecall-moji-esp32s3` | GC9A01，240 × 240 | 通用面板适配器 | Boot |
| `kevin-sp-v4-dev` | ST7789，240 × 280 | 通用 ST7789 | Boot |
| `du-chatx` | ST7789，128 × 160 | 通用 ST7789 紧凑布局 | Boot |
| `minsi-k08-dual` | ST7789，240 × 240 | 通用 ST7789 | Boot 与音量键 |
| `magiclick-2p5` | GC9107 或 ST7735，128 × 128 | 通用紧凑面板适配器 | 三个按键 |
| `atk-dnesp32s3-box3` | ST7789，320 × 240 | 通用 ST7789 | Boot |
| `lilygo-t-display-s3-pro-mvsrlora` | ST7796，222 × 480 | 通用面板适配器 | CST226SE 与 Boot |
| `atk-dnesp32s3-box` | i80 ST7789，320 × 240 | 通用 ST7789 | Boot |
| `tudouzi` | 无屏 | 无 | Boot 与两个音量键 |

无屏板型选择 `Kconfig.headless`，不编入 LVGL。ML307、Wi-Fi 及音视频协议与显示后端相互独立。
行者 Cube 使用 SPI3 mode 3 / 40 MHz、零偏移与 GPIO13 LEDC 背光，PWM 为 5 kHz、13-bit、
60% 占空比。
Bread Compact Wi-Fi LCD 使用 SPI3 mode 0 / 40 MHz、零偏移、RGB 顺序和颜色反转，以及
GPIO42 LEDC 背光（5 kHz、13-bit、60% 占空比）。仅有一个 Boot 输入，共享界面按竖屏尺寸
布局。
AMOLED 2.16 使用零 panel gap 与 40 MHz QSPI；CO5300 传输区域两个方向均保持偶数像素
边界。
AMOLED 1.8 V2 使用 16 像素 X 偏移与 40 MHz QSPI，并保持相同的 CO5300 偶数像素约束。
LCD 1.85 使用 GPIO5 LEDC 背光（60%）与 360 × 360 ST77916 面板。GenjuTech 1.54 使用
ST7789 SPI3 mode 3 / 80 MHz、颜色反转与 GPIO2 LEDC 背光（60%）。
征辰 Camera 使用 ST7789 SPI3 mode 0 / 80 MHz、固定 320 × 240 横屏与低电平有效的 GPIO42
背光。两款 LabPlus 使用 SPI2 / 20 MHz 的 320 × 172 大端面板；掌控板 V3 的 CS 为 GPIO34、
背光为 GPIO33，乐动掌控 V2 不使用 CS 引脚、背光为 GPIO34，并通过 STM8 保持面板复位时序。
ATK DNESP32S3 使用 ST7789 SPI2 mode 0 / 20 MHz、大端像素和 TCA9555 控制的 LCD 电源。
Yunliao S3 使用 ST7789 SPI2 mode 0 / 40 MHz 与颜色反转。T-CameraPlus S3 使用 240 × 240
ST7789 / 60 MHz，两个 profile 的触摸、麦克风和扬声器使能引脚不同。T-Circle S3 使用 SPI3
40 MHz 的 160 × 160 GC9D01N 圆屏。Jiuchuan S3 使用 SPI3 mode 3 / 80 MHz 的 240 × 296
GC9309NA 面板与 BGR 顺序。
KEVIN SP V4 Dev 使用 ST7789 SPI3 mode 3 / 80 MHz、Y 偏移 20、颜色反转与 GPIO48 LEDC
背光。DuChat X 使用 ST7789 SPI3 mode 0 / 40 MHz、2×1 像素偏移、X/Y 镜像与 GPIO9 LEDC
背光。
Minsi K08 使用 ST7789 SPI3 mode 3 / 40 MHz、X 偏移 80、交换 X/Y 与 GPIO7 LEDC 背光。
Magiclick 2.5 通过 GPIO3 ADC 选择 GC9107 或 ST7735，并使用 GPIO13 LEDC 背光。
ATK Box 3 使用 SPI2 / 60 MHz 的 ST7789 与大端像素数据，并由 AW9523B bit 8 二值开关背光。
T-Display S3 Pro 使用 SPI3 / 40 MHz 的 222 × 480 ST7796、49 像素 X 偏移和 CST226SE 直连
I2C 输入。ATK DNESP32S3 Box 使用 10 MHz 的 8-bit i80 ST7789 总线，扩展器负责的电源与背光
路径不在面板驱动内。两款布局目前只完成构建验证；面板方向、颜色和触摸坐标仍需真机确认。

## CoreS3像素眼镜视图

仅CoreS3使用固定工艺灰皮肤、23表情及四行终端，正文9 px、签名10 px，六位数字配对码
固定32 px。READY为NEUTRAL；进入会话SURPRISED保持300 ms并另计进入过渡。LISTENING、
THINKING显示对应表情，连续思考三秒后转PROCESSING，SPEAKING/无阶段时NEUTRAL。
当前SDK不再提供一次性情绪指示，MCP设备命令仅支持`audio.set_volume`与
`audio.adjust_volume`。仅控制LCD，具体规则、资源与验证见
[CoreS3 UI](CORES3_UI.zh-CN.md)。

CoreS3保留浅色主题字段兼容但不改变像素皮肤；动画开关控制表情运动/切换，关闭后仍推进
开场保持并保留必要的引导文字滚动。其他profile保留原UI/硬件；摄像头/视频策略、
输入、电源及内存配置不变。

## 其他带屏板型的共享流程视图

界面接收 SDK 语义 LCD 内容：十种流程页面、配对码、声纹指示以及聆听/思考/说话状态。
坐标根据各面板逻辑宽高计算，不假设 CoreS3 的 320 × 240。少量本地状态图像预解码到
Flash，不加入运行时 PNG/GIF 解码器或全屏缓存。

配网页面中央显示实际设备 SoftAP 名称（`mybot-xxxx`），底部保留中英文连接热点提示。
标题与提示仅在超出标签宽度时循环滚动，短文字静止，退出配网后停止滚动。
`CONFIG_MYBOT_LVGL_UI_ANIMATIONS` 只控制状态活动动画，关闭后仍允许必要的文字滚动。

配对码页面提示用户在网页控制台输入配对码；提示超出底部标签宽度时才循环滚动，离开
配对码页面后停止滚动。

宽度小于 240 像素的屏幕保留 20 px 中文字库；英文状态、通知及普通
标签使用 10 px 字体。配网 SSID 在两种语言下均使用 20 px，超宽时滚动。配对码根据宽度
选择 32、20 或 10 px 字体，窄屏长配对码同时收紧字距。内置中文字库覆盖 UI 文案，
不覆盖任意中文聊天正文。

触摸和按键事件仍由各板输入驱动维护。UI 不增加手势，不改变配网或音视频生命周期。
后端使用一条 16 行 RGB565 DMA 条带（`宽度 × 16 × 2` 字节），LVGL 任务固定运行于 Core 1，
优先级为 1。对象及绘制临时空间使用 PSRAM，DMA 条带使用内部内存；CoreS3 条带为 10,240
字节，StickS3 为 4,320 字节，AMOLED 2.16 为 15,360 字节，任务栈和驱动分配另计。
硬件集成负责复位、偏移、颜色顺序、
背光和销毁。

圆屏 profile 设置 `MYBOT_DISPLAY_ROUND=1`，共享视图会把标题栏、状态卡和页脚收窄到中央
安全矩形，避免控件落入圆角区域。当前只完成构建验证；颜色、方向、裁切和触摸坐标仍需逐板
真机确认。

## 编译示例

不同板型和语言请使用独立构建目录，无需额外启用 UI：

```sh
idf.py -B build/esp-vocat-lvgl \
  -DMYBOT_BOARD=esp-vocat \
  -DSDKCONFIG=build/esp-vocat-lvgl/sdkconfig \
  -DSDKCONFIG_DEFAULTS=sdkconfig.defaults build
```

主题和活动动画仍为编译选项，不新增运行时主题手势。共享视图浅色主题可追加
`ci/lvgl-ui-light.defaults`，静态状态指示可追加 `ci/lvgl-ui-static.defaults`；CoreS3即使
追加浅色defaults也保持工艺灰，静态defaults关闭其表情动画。修改defaults使用新sdkconfig，
defaults不会覆盖已有保存值。

## 验证状态

当前构建与真机状态统一记录在[验证矩阵](VALIDATION.md)中。宿主适配器测试覆盖生命周期、
DMA、内容复制和销毁行为，但不覆盖完整 LVGL 页面渲染、面板行为或硬件集成；这些仍需完整
LVGL 页面渲染检查和真机回归。
