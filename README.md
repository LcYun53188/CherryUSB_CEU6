# CherryUSB to SBUS Controller Converter (STM32F411)

本项目基于 STM32F411CEU6 开发板，利用 **CherryUSB** 协议栈实现 USB Host 功能，将接入的 USB 控制器（如 Xbox 手柄、键鼠等）信号转换为标准 **SBUS** 协议输出。

## 🚀 核心功能
- **USB Host 支持**：集成 CherryUSB 协议栈，通过开发板自带的 USB 接口，目前支持 Xbox 手柄、HID 鼠标和键盘的接入。
- **协议转换**：解析手柄摇杆与按键数据，实时封装为 25 字节的标准 SBUS 帧。
- **多路输出**：
  - **标准 SBUS**：USART2 输出，100k 波特率，直接对接飞控或其他接收机接口。
  - **普通串口**：USART1 输出，115200 波特率，用于常规标准数据帧输出。

## 🛠️ 硬件参数
- **微控制器**: STM32F411CEU6 (Black Pill)
- **内核频率**: 96MHz (USB 时钟 48MHz)
- **操作系统**: FreeRTOS (CMSIS-OS V2)

## 📚 软件依赖
- **[CherryUSB](https://github.com/cherry-cpu/CherryUSB)**: 版本 **v1.6.0** (Host Stack)

### 引脚定义 (Pinout)
| 功能模块 | 引脚 | 描述 |
| :--- | :--- | :--- |
| **USB 接口** | Type-C | STM32F411CEU6 开发板自带 USB 接口 (需外部 5V 供电或通过 VBUS 供电) |
| **SBUS_TX** | **PA2** | USART2 输出 (100k, 9-E-2) |
| **UART1_TX** | **PA9** | USART1 普通串口输出 (115200, 8-N-1) |

## 🎮 信号与控制映射 (全新自定义层)

通过引入独立的 `app_mapper.c/h` 映射层，现已实现手柄输入与 SBUS 输出的**完全解耦与高度自定义**。

### 如何自定义映射？
所有的通道绑定均在 `Core/Src/app_mapper.c` 中的 `sbus_map_config[16]` 数组中进行。你可以使用极其简明的宏来随意调配 16 个 SBUS 通道：
- **`MAP_AXIS(axis, out_min, out_max)`**：将摇杆/扳机映射为模拟通道。
  - *例子：`MAP_AXIS(GAMEPAD_AXIS_LX, 200, 1800)`*。
  - *反向：互换输出极值即可，如 `MAP_AXIS(GAMEPAD_AXIS_LX, 1800, 200)`。*
- **`MAP_BUTTON(btn, val_pressed, val_released)`**：将按键映射为数字/模拟极值。
  - *例子：`MAP_BUTTON(GAMEPAD_BTN_A, 1800, 200)`*。
- **`MAP_NONE()`**：通道留空，默认稳定在 1024。

> **提示**：目前底层解析引擎会自动获取所有手柄 16 种按键以及 6 种摇杆/扳机值的 -32768 ~ 32767 全带宽原生数据，并执行安全的钳位和映射计算。

## 📡 SBUS 协议说明
本项目严格遵循 Futaba SBUS 协议输出数据，具备如下特有标准：
- **帧长**：固定为 25 字节（包含 `0x0F` 帧头，22 字节被高度压缩的 16路×11bit 通道负载，1字节标志位，以及 `0x00` 帧尾）。
- **物理通讯参数**：强制使用 **100k 波特率、8位数据、偶校验(Even Parity)、2停止位**。
- **外部非门（反相）注意事项**：原生 SBUS 协议是“逻辑反相”的（空闲为低电平）。由于 STM32F411 的硬件 UART 默认不具备内置电平反相功能，若要对接标准的飞控 SBUS 引脚，可能需要**外置硬件反相器**（如 74HC04 逻辑芯片或使用 NPN 三极管搭建的反相电路），或使用飞控自带反相的接收焊盘。

## 📅 后续开发计划 (Roadmap)
- [ ] **全键鼠集成**: 完成 HID 鼠标移动量 (`dx`, `dy`) 和键盘按键到 SBUS 通道的细节映射。
- [x] **架构解耦**: 实现纯代码宏驱动的信号映射层体系 (`app_mapper`)。
- [x] **DMA 发送优化**: 升级 UART 传输模式为 DMA，减少高频发送时的 CPU 中断负载。
- [ ] **参数配置系统**: 实现支持 Flash 存储的串口 CLI，允许用户上电后免编译动态下发指令调整映射。
- [ ] **失控保护 (Failsafe)**：当 USB 手柄拔出或无事件时，自动向 SBUS 帧注入标志位。

## 📦 编译与运行
项目使用 Keil MDK-ARM 进行开发：
1. 打开 `MDK-ARM/CherryUSB_CEU6.uvprojx`。
2. 确保已正确包含 CherryUSB 源码路径。
3. 编译并烧录至 STM32F411CEU6。
