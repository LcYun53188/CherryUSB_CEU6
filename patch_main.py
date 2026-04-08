import re

main_path = 'E:/Project/CherryUSB_F411/CherryUSB_CEU6/Core/Src/main.c'
with open(main_path, 'r', encoding='utf-8') as f:
    content = f.read()

# Includes
content = re.sub(r'/\* USER CODE BEGIN Includes \*/.*?/\* USER CODE END Includes \*/', 
                 '/* USER CODE BEGIN Includes */\n#include "usbh_core.h"\n#include "usbh_hid.h"\n#include "usbh_xbox.h"\n#include "app_sbus.h"\n#include <stdio.h>\n/* USER CODE END Includes */', 
                 content, flags=re.DOTALL)

# PV
content = re.sub(r'/\* USER CODE BEGIN PV \*/.*?/\* USER CODE END PV \*/', 
                 '/* USER CODE BEGIN PV */\nUART_HandleTypeDef huart1;\nUART_HandleTypeDef huart2;\n\nuint16_t sbus_channels[16];\nuint8_t sbus_frame[25];\n\nvolatile uint16_t pad_lx = 1024, pad_ly = 1024, pad_rx = 1024, pad_ry = 1024;\nvolatile uint16_t pad_buttons = 0;\nvolatile int8_t mouse_dx = 0, mouse_dy = 0;\n/* USER CODE END PV */', 
                 content, flags=re.DOTALL)

# PFP
content = re.sub(r'/\* USER CODE BEGIN PFP \*/.*?/\* USER CODE END PFP \*/', 
                 '/* USER CODE BEGIN PFP */\nstatic void MX_USART1_UART_Init(void);\nstatic void MX_USART2_UART_Init(void);\n\n#ifdef __GNUC__\n#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)\n#else\n#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)\n#endif\n/* USER CODE END PFP */', 
                 content, flags=re.DOTALL)

# 0
content = re.sub(r'/\* USER CODE BEGIN 0 \*/.*?/\* USER CODE END 0 \*/', 
                 """/* USER CODE BEGIN 0 */
PUTCHAR_PROTOTYPE
{
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
  return ch;
}

void usbh_hid_keyboard_callback(void *arg, int nbytes)
{
    struct usbh_hid *hid_class = (struct usbh_hid *)arg;
    if (nbytes > 0) {
        uint8_t *buf = hid_class->intin_buffer;
        printf("Keyboard: Mod:0x%02x Keys: %02x %02x %02x\\r\\n", buf[0], buf[2], buf[3], buf[4]);
    }
    usbh_submit_urb(&hid_class->intin_urb);
}

void usbh_hid_mouse_callback(void *arg, int nbytes)
{
    struct usbh_hid *hid_class = (struct usbh_hid *)arg;
    if (nbytes > 0) {
        uint8_t *buf = hid_class->intin_buffer;
        mouse_dx = (int8_t)buf[1];
        mouse_dy = (int8_t)buf[2];
        printf("Mouse: dx:%d dy:%d btn:%d\\r\\n", mouse_dx, mouse_dy, buf[0]);
    }
    usbh_submit_urb(&hid_class->intin_urb);
}

void usbh_xbox_callback(void *arg, int nbytes)
{
    struct usbh_xbox *xbox_class = (struct usbh_xbox *)arg;
    if (nbytes > 0) {
        uint8_t *buf = xbox_class->intin_buffer;
        if(buf[0] == 0x00 && nbytes >= 14) {
            pad_buttons = (buf[3] << 8) | buf[2];
            int16_t lx = (int16_t)((buf[7] << 8) | buf[6]);
            int16_t ly = (int16_t)((buf[9] << 8) | buf[8]);
            int16_t rx = (int16_t)((buf[11] << 8) | buf[10]);
            int16_t ry = (int16_t)((buf[13] << 8) | buf[12]);
            
            pad_lx = (lx / 32) + 1024;
            pad_ly = (ly / 32) + 1024;
            pad_rx = (rx / 32) + 1024;
            pad_ry = (ry / 32) + 1024;

            printf("Xbox: LX:%4d LY:%4d BTN:%04x\\r\\n", pad_lx, pad_ly, pad_buttons);
        }
    }
    usbh_submit_urb(&xbox_class->intin_urb);
}
/* USER CODE END 0 */""", 
                 content, flags=re.DOTALL)

# 2
content = re.sub(r'/\* USER CODE BEGIN 2 \*/.*?/\* USER CODE END 2 \*/', 
                 """/* USER CODE BEGIN 2 */
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();

  printf("STM32F411 CherryUSB Host RTOS Initialize...\\r\\n");
  for(int i=0; i<16; i++) sbus_channels[i] = 1024;
  
  usbh_initialize(0, USB_OTG_FS_PERIPH_BASE);
/* USER CODE END 2 */""", 
                 content, flags=re.DOTALL)

# 4
content = re.sub(r'/\* USER CODE BEGIN 4 \*/.*?/\* USER CODE END 4 \*/', 
                 """/* USER CODE BEGIN 4 */
static void MX_USART1_UART_Init(void)
{
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK) { Error_Handler(); }
}

static void MX_USART2_UART_Init(void)
{
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 100000;
  huart2.Init.WordLength = UART_WORDLENGTH_9B;
  huart2.Init.StopBits = UART_STOPBITS_2;
  huart2.Init.Parity = UART_PARITY_EVEN;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK) { Error_Handler(); }
}
/* USER CODE END 4 */""", 
                 content, flags=re.DOTALL)

# 5
content = re.sub(r'/\* USER CODE BEGIN 5 \*/.*?/\* USER CODE END 5 \*/', 
                 """/* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
    sbus_channels[0] = pad_lx;
    sbus_channels[1] = pad_ly;
    sbus_channels[2] = pad_rx;
    sbus_channels[3] = pad_ry;
    sbus_channels[4] = (pad_buttons & 0x1000) ? 1800 : 200; // A
    sbus_channels[5] = (pad_buttons & 0x2000) ? 1800 : 200; // B
    sbus_channels[6] = (pad_buttons & 0x4000) ? 1800 : 200; // X
    sbus_channels[7] = (pad_buttons & 0x8000) ? 1800 : 200; // Y

    sbus_build_frame(sbus_frame, sbus_channels);
    HAL_UART_Transmit(&huart2, sbus_frame, 25, 5); // Output SBUS

    osDelay(10); // 10ms loop
  }
  /* USER CODE END 5 */""", 
                 content, flags=re.DOTALL)

# Fix SystemClock_Config since CubeMX wiped the 96MHz settings because no HSE was checked in GUI
content = re.sub(r'void SystemClock_Config\(void\)\n\{.*?\n\}\n', 
                 """void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}
""", 
                 content, flags=re.DOTALL)

with open(main_path, 'w', encoding='utf-8') as f:
    f.write(content)

print("main.c rewritten successfully")
