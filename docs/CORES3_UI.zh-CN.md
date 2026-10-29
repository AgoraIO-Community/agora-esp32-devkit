# CoreS3 工艺灰像素眼镜 UI

[English](CORES3_UI.md) | [简体中文](CORES3_UI.zh-CN.md)

`MYBOT_BOARD=m5stack-core-s3` 自动为320 × 240 ILI9342屏选择工艺灰像素眼镜界面，复用原
CoreS3 LVGL适配器及公共LCD视图接口。不增加手工渲染器开关，无需覆盖隐藏的
`CONFIG_MYBOT_LVGL_UI`。其他Board profile保留原UI与硬件行为，见
[平台UI](PLATFORM_UI.zh-CN.md)。

本视图替换CoreS3旧状态卡片。音频、摄像头/视频策略、FT6336触摸手势、电源、引脚、PSRAM
和分区仍采用原板级配置；短触开始/结束会话、长按三秒配网保持。agent-studio-device-firmware SDK、AOSL与Agora
RTSA快照不变。

## 显示内容

界面采用 `process`（工艺灰）皮肤与 `geek`（像素眼镜），使用固定 `#050708` 黑色
玻璃底、灰色装饰及像素镜框。四行终端显示产品标识、表情名称、运行/会话状态及realtime
签名，正文使用9 px字库，`</>`使用10 px。会话状态行显示聆听/思考/说话及
`vp:ok`或`vp:pending`；声纹变化不额外弹通知。图案表示语义状态，不测量麦克风幅度。

仍支持SDK全部十种流程页。启动、配网、断网、服务启动、配对、配对码、错误和停止保留
中英文必要引导。配网显示实际启用的SoftAP SSID，无法取得时使用通用配网标题。
超宽SSID/提示仍按需滚动，关闭表情动画后也可滚动，离开对应引导页面停止。

配对页提示在网页控制台输入配对码。CoreS3六位数字配对码固定使用32 px字体，不缩小适配。
中文引导复用原20 px思源黑体子集，字形覆盖产品文案，不覆盖任意中文聊天正文。

## 自动表情

渲染器根据SDK流程页面和服务端状态指示选择表情。当前公共LCD接口不提供一次性情绪指示；
MCP通路仅接受`message.user`封装内的`audio.set_volume`与`audio.adjust_volume`。

| SDK状态 | 表情与保持时间 |
| --- | --- |
| READY / 挂断返回 | NEUTRAL |
| 进入会话 | SURPRISED保持300 ms，另计进入过渡 |
| LISTENING | LISTENING |
| THINKING | THINKING；连续三秒后PROCESSING |
| SPEAKING / 无阶段 | NEUTRAL |

开启动画时，每次保持另加250 ms进入过渡，会话开场同样另计；关闭动画时只有保持时间。
普通阶段/声纹更新不重置开场保持计时，到期恢复最新自动表情；后台思考已满三秒时恢复
PROCESSING。离开会话或SDK解挂会清除表情计时，必要引导优先。全部表情仅控制LCD，
不发送电机命令。

完整23表情如下：

```text
NEUTRAL, GREETING, HAPPY, EXCITED, CURIOUS, CONFUSED, THINKING, LISTENING,
SURPRISED, SHY, PROUD, LOVE, SLEEPY, BORED, SAD, PLEADING, APOLOGY, WORRIED,
ANNOYED, REFUSE, PROCESSING, ERROR, SLEEP
```

预置图案保留，不自动演示轮播。ERROR和SLEEP图案不改变SDK运行状态，也不
使设备休眠。CoreS3适配器在原有复制投递LCD状态流程中维护表情计时，现有LVGL任务渲染，
不新增UI线程或UTC时钟依赖。

## 主题与动画

| 配置 | CoreS3行为 |
| --- | --- |
| `CONFIG_MYBOT_LVGL_UI_LIGHT_THEME` | 保留已有配置兼容；像素视图始终使用工艺灰 |
| `CONFIG_MYBOT_LVGL_UI_ANIMATIONS` | 控制表情运动、250 ms切换及光标动画；关闭后保留状态变化、保持到期和必要引导滚动 |

其他带屏板型继续使用共享流程视图的明暗主题与活动动画规则。不增加切换主题或选择表情的
触摸手势。

## 编译与烧录

使用ESP-IDF v5.5.2。标准构建脚本选择CoreS3与中英文，默认 `--video auto` 根据既有板级
能力开启CoreS3视频：

```sh
python3 scripts/build_all.py --board m5stack-core-s3 --language both \
  --build-root /tmp/mybot-cores3-pixel-build \
  --output-root /tmp/mybot-cores3-pixel-firmware
```

中间产物与固件写入上述专用且互不嵌套的目录；路径不得包含 `..` 或符号链接分量。
macOS 请将示例中的 `/tmp/` 别名换成 `/private/tmp/`。仓库内的根目录仅允许位于
`build/` 或 `releases/` 目录树中。

脚本为新建或空目录写入 `.mybot-generated-root.json` 标记，默认先清理这些目录。
已有非空目录若缺少匹配的标记，即使使用 `--no-clean` 也会拒绝。请检查旧目录并将其移到
其他位置，或使用新目录，不要手工创建标记。`--no-clean` 保留数据但仍检查归属；
`--dry-run` 执行相同检查，不创建目录或标记。清理需要 POSIX 安全目录操作和 Python 3.11
及以上版本，不满足时拒绝清理。`--video off` 用于无视频对比；这些选项均不切换渲染器。

直接使用IDF时按所选sdkconfig/defaults构建，不包含脚本额外追加的自动视频配置，例如：

```sh
idf.py -B build/cores3-pixel \
  -DMYBOT_BOARD=m5stack-core-s3 \
  -DSDKCONFIG=build/cores3-pixel/sdkconfig \
  -DSDKCONFIG_DEFAULTS=sdkconfig.defaults build
idf.py -B build/cores3-pixel -p <PORT> flash monitor
```

语言、视频、动画对比使用独立sdkconfig，defaults不会覆盖已保存值；构建验证单独记录于下文。

## 渲染、资源与来源

现有LVGL任务运行于Core 1、优先级1。7,168字节任务栈现通过已有port的 `task_stack_caps`
放入PSRAM，减少7 KiB内部栈存储。平台内存策略自动选择
`CONFIG_FREERTOS_TASK_CREATE_ALLOW_EXT_MEM=y`。
对象与绘制临时空间也使用PSRAM，SPI继续使用320 × 16 RGB565条带，即10,240字节
内部DMA内存；驱动和同步资源另计。
视图状态、两组形状数据和被标签保留的字符串共同分配在 PSRAM，LVGL 控件删除后再释放。
局部刷新不增加全屏页面缓存或整脸解码缓冲；完整清单见[PSRAM 内存策略](PSRAM_MEMORY.zh-CN.md)。

显示初始化日志包含 `ui_stack_bytes=7168 ui_stack=psram`。搬栈减少内部内存占用，不能保证
摄像头所需的最大DMA连续块；需结合[视频说明](CORES3_VIDEO.zh-CN.md)中的
`event=video_memory`检查，不能只凭空闲内存总量判断采集能够启动。

固定LVGL控件和曲线绘制表情，五个不可变A8蒙版保存心形、星形、椭圆、脸颊与眼泪，
alpha数据合计13,631字节，不含描述符。终端字库由DejaVu Sans Mono Bold派生并重命名，
保留Bitstream Vera许可；思源黑体UI子集仍为SIL OFL 1.1。蒙版与终端字库仅编入CoreS3
像素渲染器，其他板型仍用原状态卡片图像。

显示实现及生成器基于BK7259像素眼镜渲染方案适配ESP32。
[已发布的ESP32实现基线](https://github.com/AgoraIO-Community/agora-esp32-devkit/tree/1bc740a8f4c5e3ba188071d50a1623d9fe328294/components/mybot_platform/src/drivers/display/renderers/lvgl)
提供已交付的适配代码，并保留各文件MIT/Apache-2.0声明及既有显示许可。
已交付的[渲染器与资源说明](../components/mybot_platform/src/drivers/display/renderers/lvgl/PIXEL_GLASSES_SOURCES.md)
列出几何、颜色、蒙版、生成器及只读资源核验命令。
对应的当前官方固件工程为
[agora-r2-standard-devkit](https://github.com/AgoraIO-Community/agora-r2-standard-devkit)。
详情见[依赖基线](../VENDORED_SOURCES.md)及[第三方声明](../THIRD_PARTY_NOTICES.md)。

## 验证状态

首轮CoreS3中英文通过ESP-IDF v5.5.2标准脚本构建，自动开启视频、60 ms音频包长：
zh-CN耗时38秒、en-US耗时37秒，总计1分16秒。这是构建验证，不是硬件结果；宿主预览与
真机回归仍需分别检查。此前CoreS3真机结果属于旧状态卡片及已有音频/摄像头/输入通路，
不代表新像素渲染器通过。仍需完成：

- 在320 × 240屏上检查两种语言、十种引导页面及23个表情。
- 检查四行独立终端、签名及固定大号六位配对码。
- 验证SURPRISED开场、阶段变化、三秒思考超时及开场保持到期；阶段/声纹更新不得重置
  开场保持计时。
- 开关动画，检查超宽SSID/提示滚动及网页控制台配对。
- 回归短触、三秒配网、断网恢复和反复SDK启停。
- 视频关闭及原1 fps摄像头上行开启时分别验证全双工音频。
- PSRAM UI栈改动后的音频、视频与UI并行，以及配网、音量持久化NVS写入回归仍待真机验证。
- 记录CPU、内部/PSRAM峰值、最小空闲堆及最大连续块，多轮会话观察资源释放和内存增长。

结果需包含板型、固件语言、视频/动画配置、音量及串口日志。
