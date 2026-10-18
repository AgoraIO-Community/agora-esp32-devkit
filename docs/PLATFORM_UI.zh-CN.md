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
| `sensecap-watcher` | SPD2010，412 × 412 | 通用面板适配器 | 原有旋钮路径 |

AtomEchoS3R 与 ReSpeaker Flex 没有 LCD，不编入 LVGL。ML307、Wi-Fi 及音视频协议与显示后端
相互独立。
行者 Cube 使用 SPI3 mode 3 / 40 MHz、零偏移与 GPIO13 LEDC 背光，PWM 为 5 kHz、13-bit、
60% 占空比；显示和电源行为仍需
真机验证。
Bread Compact Wi-Fi LCD 使用 SPI3 mode 0 / 40 MHz、零偏移、RGB 顺序和颜色反转，以及
GPIO42 LEDC 背光（5 kHz、13-bit、60% 占空比）。仅有一个 Boot 输入，共享界面按竖屏尺寸
布局；中英文构建支持已验证，真机验证仍未完成。
AMOLED 2.16 使用零 panel gap 与 40 MHz QSPI；CO5300 传输区域两个方向均保持偶数像素
边界，触摸和面板方向必须在实际板卡上验证。
2.16 中英文构建支持已验证，真机测试仍未完成。

## CoreS3像素眼镜视图

仅CoreS3使用固定工艺灰皮肤、23表情及四行终端，正文9 px、签名10 px，六位数字配对码
固定32 px。READY为NEUTRAL；进入会话SURPRISED保持300 ms并另计进入过渡。LISTENING、
THINKING显示对应表情，连续思考三秒后转PROCESSING，SPEAKING/无阶段时NEUTRAL。
四种可选的一次性LCD情绪指示临时覆盖当前图案，每次提交都会重播，分别保持
2000/3000/1200/800 ms；开启动画时另加250 ms进入过渡。当前SDK MCP通路仅接受
`message.user`封装内的`audio.set_volume`与`audio.adjust_volume`，不产生情绪指示。
仅控制LCD，具体规则、资源与验证见[CoreS3 UI](CORES3_UI.zh-CN.md)。

CoreS3保留浅色主题字段兼容但不改变像素皮肤；动画开关控制表情运动/切换，关闭后仍推进
状态与保持到期，并保留必要的引导文字滚动。其他十二个profile保留原UI/硬件；SDK、AOSL、
RTSA、摄像头/视频策略、输入、电源及内存配置不变。

## 其他带屏板型的共享流程视图

界面接收 SDK 语义 LCD 内容：十种流程页面、配对码、声纹指示以及聆听/思考/说话状态。
坐标根据各面板逻辑宽高计算，不假设 CoreS3 的 320 × 240。少量本地状态图像预解码到
Flash，不加入运行时 PNG/GIF 解码器或全屏缓存。

配网页面中央显示实际设备 SoftAP 名称（`mybot-xxxx`），底部保留中英文连接热点提示。
标题与提示仅在超出标签宽度时循环滚动，短文字静止，退出配网后停止滚动。
`CONFIG_MYBOT_LVGL_UI_ANIMATIONS` 只控制状态活动动画，关闭后仍允许必要的文字滚动。

配对码页面提示用户在网页控制台输入配对码；提示超出底部标签宽度时才循环滚动，离开
配对码页面后停止滚动。

宽度小于 240 像素的屏幕（当前为 StickS3）保留 20 px 中文字库；英文状态、通知及普通
标签使用 10 px 字体。配网 SSID 在两种语言下均使用 20 px，超宽时滚动。配对码根据宽度
选择 32、20 或 10 px 字体，窄屏长配对码同时收紧字距。内置中文字库覆盖 UI 文案，
不覆盖任意中文聊天正文。

触摸和按键事件仍由各板输入驱动维护。UI 不增加手势，不改变配网或音视频生命周期。
后端使用一条 16 行 RGB565 DMA 条带（`宽度 × 16 × 2` 字节），LVGL 任务固定运行于 Core 1，
优先级为 1。对象及绘制临时空间使用 PSRAM，DMA 条带使用内部内存；CoreS3 条带为 10,240
字节，StickS3 为 4,320 字节，AMOLED 2.16 为 15,360 字节，任务栈和驱动分配另计。
硬件集成负责复位、偏移、颜色顺序、
背光和销毁。

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

此前CoreS3状态卡片UI已通过真机测试；新像素眼镜已通过首轮中英文ESP-IDF v5.5.2标准脚本
视频构建，仍需宿主检查与真机验收。旧结果不能证明新排版、表情时序或资源负载通过。共享视图清理、配网热点名称/
滚动和窄屏字体仍保留既有回归要求。
CI定义十三板型中英文、CoreS3视频中英文及主题/动画变体，共30配置；矩阵定义不代表本次
修改已完成全部构建，更不代表新增板型已经通过真机颜色、
触摸、音频、配网或销毁验证。真机验收需覆盖所有流程页、方向与颜色、小屏中英文、
关闭状态动画时的热点名称/提示滚动、触摸/按键、Wi-Fi 恢复、反复 mybot 停止/启动、
全双工音频及 CPU/内存测量。
