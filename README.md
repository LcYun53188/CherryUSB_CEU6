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

## 🎮 控制映射
目前已实现 Xbox 手柄的初步映射：
- **通道 1-4**: 对应左/右摇杆 (X, Y)
- **通道 5-8**: 对应 A, B, X, Y 按键 (按下为 1800, 释放为 200)

## 📅 后续开发计划 (Roadmap)
- [ ] **全键鼠集成**: 完成 HID 鼠标移动量 (`dx`, `dy`) 和键盘按键到 SBUS 通道的细节映射。
- [ ] **DMA 发送优化**: 升级 UART 传输模式为 DMA，减少高频发送时的 CPU 中断负载。
- [ ] **USB Hub 增强**: 完善 Hub 接入逻辑，支持多个外设（如手柄+鼠标）同时工作。
- [ ] **参数配置系统**: 实现简单的串口 CLI，允许用户动态调整通道映射、极性翻转等配置。
- [ ] **失控保护 (Failsafe)**：当 USB 设备拔出或通讯丢失时，自动向 SBUS 帧注入失控标志位。

## 📦 编译与运行
项目使用 Keil MDK-ARM 进行开发：
1. 打开 `MDK-ARM/CherryUSB_CEU6.uvprojx`。
2. 确保已正确包含 CherryUSB 源码路径。
3. 编译并烧录至 STM32F411CEU6。
