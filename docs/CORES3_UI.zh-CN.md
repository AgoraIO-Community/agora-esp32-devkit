# CoreS3 工艺灰像素眼镜 UI

[English](CORES3_UI.md) | [简体中文](CORES3_UI.zh-CN.md)

`MYBOT_BOARD=m5stack-core-s3` 自动为320 × 240 ILI9342屏选择工艺灰像素眼镜界面，复用原
CoreS3 LVGL适配器及公共LCD视图接口。不增加手工渲染器开关，无需覆盖隐藏的
`CONFIG_MYBOT_LVGL_UI`。其他十二个Board profile保留原UI与硬件行为，见
[平台UI](PLATFORM_UI.zh-CN.md)。

本视图替换CoreS3旧状态卡片。音频、摄像头/视频策略、FT6336触摸手势、电源、引脚、PSRAM
和分区仍采用原板级配置；短触开始/结束会话、长按三秒配网保持。agent-studio-device-firmware SDK、AOSL与Agora
RTSA快照不变。

## 显示内容

外观参考模拟器 `process`（工艺灰）皮肤与 `geek`（像素眼镜），使用固定 `#050708` 黑色
玻璃底、灰色装饰及像素镜框。四行终端显示产品标识、表情名称、运行/会话状态及realtime
签名，正文使用9 px字库，`</>`使用10 px。会话状态行显示聆听/思考/说话及
`vp:ok`或`vp:pending`；声纹变化不额外弹通知。图案表示语义状态，不测量麦克风幅度。

仍支持SDK全部十种流程页。启动、配网、断网、服务启动、配对、配对码、错误和停止保留
中英文必要引导。配网显示实际启用的SoftAP SSID，无法取得时使用通用配网标题。
超宽SSID/提示仍按需滚动，关闭表情动画后也可滚动，离开对应引导页面停止。

配对页提示在网页控制台输入配对码。CoreS3六位数字配对码固定使用32 px字体，不缩小适配。
中文引导复用原20 px思源黑体子集，字形覆盖产品文案，不覆盖任意中文聊天正文。

## 自动表情与MCP命令

| 状态或受理的新命令 | 表情与保持时间 |
| --- | --- |
| READY / 挂断返回 | NEUTRAL |
| 进入会话 | SURPRISED保持300 ms，另计进入过渡 |
| LISTENING | LISTENING |
| THINKING | THINKING；连续三秒后PROCESSING |
| SPEAKING / 无阶段 | NEUTRAL |
| MCP `emotion.happy` | HAPPY，2000 ms |
| MCP `emotion.sad` | SAD，3000 ms |
| MCP `emotion.angry` | ANNOYED，1200 ms |
| MCP `emotion.surprised` | SURPRISED，800 ms |

开启动画时，每次保持另加250 ms进入过渡，会话开场同样另计；关闭动画时只有保持时间。
新受理MCP命令替换未结束的前景，同名新命令也重播；SDK原有command ID去重仍生效。
普通阶段/声纹更新不重置保持计时，到期恢复最新自动表情；后台思考已满三秒时恢复
PROCESSING。离开会话或SDK解挂会清除表情计时，必要引导优先。全部表情仅控制LCD，
不发送电机命令。

完整23表情如下：

```text
NEUTRAL, GREETING, HAPPY, EXCITED, CURIOUS, CONFUSED, THINKING, LISTENING,
SURPRISED, SHY, PROUD, LOVE, SLEEPY, BORED, SAD, PLEADING, APOLOGY, WORRIED,
ANNOYED, REFUSE, PROCESSING, ERROR, SLEEP
```

GREETING及其余预置图案保留，不自动演示轮播。ERROR和SLEEP图案不改变SDK运行状态，也不
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

中间产物与固件写入上述独立目录；脚本默认清理所选输出目录，增量构建可加 `--no-clean`。
`--dry-run`仅打印计划，`--video off`用于无视频对比；这些选项均不切换渲染器。

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

现有LVGL任务运行于Core 1、优先级1。对象与绘制临时空间使用PSRAM，SPI继续使用
320 × 16 RGB565条带，即10,240字节内部DMA内存；UI线程栈、驱动和同步资源另计。
局部刷新不增加全屏页面缓存或整脸解码缓冲。

固定LVGL控件和曲线绘制表情，五个不可变A8蒙版保存心形、星形、椭圆、脸颊与眼泪，
alpha数据合计13,631字节，不含描述符。终端字库由DejaVu Sans Mono Bold派生并重命名，
保留Bitstream Vera许可；思源黑体UI子集仍为SIL OFL 1.1。蒙版与终端字库仅编入CoreS3
像素渲染器，其他板型仍用原状态卡片图像。

显示实现及生成器来源为 `mybot-bk7259` 中的 `bk_solution_ai`（`github.com/junlon2006/bk_solution_ai`）提交
`dc4c85492bcf00f780c92d56c6cba2a362f54f68`，保留各文件MIT/Apache-2.0声明及既有显示许可。
视觉/几何参考用户提供的threejs模拟器，未见模拟器独立许可证声明，不为其另行指定许可。
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
- 验证SURPRISED开场、阶段变化、三秒思考超时、MCP替换、同名新命令重播及保持到期。
- 开关动画，检查超宽SSID/提示滚动及网页控制台配对。
- 回归短触、三秒配网、断网恢复和反复SDK启停。
- 视频关闭及原1 fps摄像头上行开启时分别验证全双工音频。
- 记录CPU、内部/PSRAM峰值、最小空闲堆及最大连续块，多轮会话观察资源释放和内存增长。

结果需包含板型、固件语言、视频/动画配置、音量及串口日志。
