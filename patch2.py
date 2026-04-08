import os

projx_path = 'E:/Project/CherryUSB_F411/CherryUSB_CEU6/MDK-ARM/CherryUSB_CEU6.uvprojx'

with open(projx_path, 'r', encoding='utf-8') as f:
    content = f.read()

target = """            <File>
              <FileName>usb_hc_dwc2.c</FileName>
              <FileType>1</FileType>
              <FilePath>../CherryUSB/port/dwc2/usb_hc_dwc2.c</FilePath>
            </File>"""

new_target = target + """
            <File>
              <FileName>usb_glue_st.c</FileName>
              <FileType>1</FileType>
              <FilePath>../CherryUSB/port/dwc2/usb_glue_st.c</FilePath>
            </File>"""

if "usb_glue_st.c" not in content:
    content = content.replace(target, new_target)
    with open(projx_path, 'w', encoding='utf-8') as f:
        f.write(content)
    print("usb_glue_st.c added correctly.")
else:
    print("usb_glue_st.c already present.")
