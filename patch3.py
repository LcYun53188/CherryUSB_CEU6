import re

# Fix UVProjx Include paths
projx_path = 'E:/Project/CherryUSB_F411/CherryUSB_CEU6/MDK-ARM/CherryUSB_CEU6.uvprojx'
with open(projx_path, 'r', encoding='utf-8') as f:
    content = f.read()

# Make sure GCC portable include path is explicitly in all <IncludePath>
def fix_includes(match):
    inc = match.group(0)
    inc = inc.replace('RVDS\\ARM_CM4F', 'GCC/ARM_CM4F')
    inc = inc.replace('RVDS/ARM_CM4F', 'GCC/ARM_CM4F')
    if "Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F" not in inc and "Middlewares\\Third_Party\\FreeRTOS\\Source" in inc:
        inc = inc.replace("</IncludePath>", ";../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F</IncludePath>")
    return inc

content = re.sub(r'<IncludePath>([^<]*)</IncludePath>', fix_includes, content)

with open(projx_path, 'w', encoding='utf-8') as f:
    f.write(content)

# Fix MSP c file
msp_path = 'E:/Project/CherryUSB_F411/CherryUSB_CEU6/Core/Src/stm32f4xx_hal_msp.c'
with open(msp_path, 'r', encoding='utf-8') as f:
    msp_content = f.read()

# We completely restore the void HAL_MspInit(void) to its CubeMX generated pristine form
# and then inject our code purely at USER CODE BEGIN 1
msp_content = re.sub(r'void HAL_MspInit\(void\).*?/\* USER CODE BEGIN 1 \*/', 
"""void HAL_MspInit(void)
{
  /* USER CODE BEGIN MspInit 0 */

  /* USER CODE END MspInit 0 */

  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();

  /* System interrupt init*/
  /* PendSV_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(PendSV_IRQn, 15, 0);

  /* USER CODE BEGIN MspInit 1 */

  /* USER CODE END MspInit 1 */
}

/* USER CODE BEGIN 1 */""", msp_content, flags=re.DOTALL)

with open(msp_path, 'w', encoding='utf-8') as f:
    f.write(msp_content)

print("Fixed uvprojx and msp")
