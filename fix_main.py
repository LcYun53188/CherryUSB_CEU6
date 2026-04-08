import re

path = 'E:/Project/CherryUSB_F411/CherryUSB_CEU6/Core/Src/main.c'
with open(path, 'r', encoding='utf-8') as f:
    content = f.read()

# Fix buffer names
content = content.replace('intin_buffer', 'intin_urb.transfer_buffer')

# Fix split string literals
content = re.sub(r'printf\("([^"]*)\n",', r'printf("\1\\n",', content)
content = re.sub(r'printf\("([^"]*)\n"\);', r'printf("\1\\n");', content)

with open(path, 'w', encoding='utf-8') as f:
    f.write(content)

print("main.c fixed syntax")
