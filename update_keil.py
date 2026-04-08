import os
import re

projx_path = 'E:/Project/CherryUSB_F411/CherryUSB_CEU6/MDK-ARM/CherryUSB_CEU6.uvprojx'

with open(projx_path, 'r', encoding='utf-8') as f:
    content = f.read()

# Add HAL drivers
target_hal_drivers = """            <File>
              <FileName>stm32f4xx_hal_exti.c</FileName>
              <FileType>1</FileType>
              <FilePath>../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_exti.c</FilePath>
            </File>"""

new_hal_drivers = target_hal_drivers + """
            <File>
              <FileName>stm32f4xx_hal_uart.c</FileName>
              <FileType>1</FileType>
              <FilePath>../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c</FilePath>
            </File>
            <File>
              <FileName>stm32f4xx_hal_hcd.c</FileName>
              <FileType>1</FileType>
              <FilePath>../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_hcd.c</FilePath>
            </File>
            <File>
              <FileName>stm32f4xx_ll_usb.c</FileName>
              <FileType>1</FileType>
              <FilePath>../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_usb.c</FilePath>
            </File>"""

# Add Include paths
target_include_re = r"<IncludePath>(.*?)</IncludePath>"
match = re.search(target_include_re, content)
if match:
    existing_includes = match.group(1)
    # Check if CherryUSB includes are present
    if "../CherryUSB/core" not in existing_includes:
        new_includes = existing_includes + ";../CherryUSB/common;../CherryUSB/core;../CherryUSB/class/hub;../CherryUSB/class/hid;../CherryUSB/class/vendor/xbox;../CherryUSB/osal"
        content = content.replace(f"<IncludePath>{existing_includes}</IncludePath>", f"<IncludePath>{new_includes}</IncludePath>")

# Add CherryUSB Group cleanly
if "<GroupName>CherryUSB</GroupName>" not in content:
    target_group = """        <Group>
          <GroupName>Drivers/CMSIS</GroupName>"""
    cherryusb_group = """        <Group>
          <GroupName>CherryUSB</GroupName>
          <Files>
            <File>
              <FileName>usb_hc_dwc2.c</FileName>
              <FileType>1</FileType>
              <FilePath>../CherryUSB/port/dwc2/usb_hc_dwc2.c</FilePath>
            </File>
            <File>
              <FileName>usb_glue_st.c</FileName>
              <FileType>1</FileType>
              <FilePath>../CherryUSB/port/dwc2/usb_glue_st.c</FilePath>
            </File>
            <File>
              <FileName>usbh_core.c</FileName>
              <FileType>1</FileType>
              <FilePath>../CherryUSB/core/usbh_core.c</FilePath>
            </File>
            <File>
              <FileName>usbh_hub.c</FileName>
              <FileType>1</FileType>
              <FilePath>../CherryUSB/class/hub/usbh_hub.c</FilePath>
            </File>
            <File>
              <FileName>usbh_hid.c</FileName>
              <FileType>1</FileType>
              <FilePath>../CherryUSB/class/hid/usbh_hid.c</FilePath>
            </File>
            <File>
              <FileName>usbh_xbox.c</FileName>
              <FileType>1</FileType>
              <FilePath>../CherryUSB/class/vendor/xbox/usbh_xbox.c</FilePath>
            </File>
            <File>
              <FileName>usb_osal_freertos.c</FileName>
              <FileType>1</FileType>
              <FilePath>../CherryUSB/osal/usb_osal_freertos.c</FilePath>
            </File>
          </Files>
        </Group>
"""
    content = content.replace(target_group, cherryusb_group + target_group)

if "<Define>USE_HAL_DRIVER,STM32F411xE</Define>" in content:
    content = content.replace("<Define>USE_HAL_DRIVER,STM32F411xE</Define>", "<Define>USE_HAL_DRIVER,STM32F411xE,USE_USB_HS_IN_FS</Define>")

with open(projx_path, 'w', encoding='utf-8') as f:
    f.write(content)

print("Keil project updated successfully!")
