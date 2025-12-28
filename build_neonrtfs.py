
import os
import subprocess
from SCons.Script import DefaultEnvironment
import re

env = DefaultEnvironment()
PROJECT_DIR = env['PROJECT_DIR']
BUILD_DIR = os.path.join(env['PROJECT_BUILD_DIR'], env['BOARD'])
HTML_DIR = os.path.join(PROJECT_DIR, "html")  # 假设 HTML 文件放在 html 文件夹中
NEONRTFS_BIN = os.path.join(PROJECT_DIR, "webpages.neonrtfs")
NEONRTFS_C = os.path.join(PROJECT_DIR, "src/NeonRTOS/HTTPd", "neonrtfs_img.c")

XXD_CMD = os.path.join(PROJECT_DIR, "xxd")  # xxd 工具路径

# 生成 .neonrtfs 文件
def generate_neonrtfs_bin():
    print("Generating webpages.neonrtfs...")
    
    # 根据操作系统选择适当的命令
    if os.name == 'nt':  # Windows
        command = f".\makeHTML_Win.bat"
    else:  # Linux 或 macOS
        command = f".\makeHTML_Unix.sh"

    # 执行命令
    result = os.system(command)
    if result == 0:
        print("webpages.neonrtfs generated successfully!")
    else:
        print("Failed to generate webpages.neonrtfs")

    try:
        # 读取 .neonrtfs 文件
        with open(NEONRTFS_BIN, "rb") as f:
            data = f.read()

        # 替换路径（注意：需要将路径转为字节格式进行替换）
        old_path_bytes = HTML_DIR.encode('utf-8')
        new_path_bytes = "".encode('utf-8')

        # 替换绝对路径为相对路径
        replaced_data = data.replace(old_path_bytes, new_path_bytes)

        # 写回修改后的文件
        with open(NEONRTFS_BIN, "wb") as f:
            f.write(replaced_data)
    except Exception as e:
        print(f"Error while replacing paths in {NEONRTFS_BIN}: {e}")

   
def generate_neonrtfs_c():
    print("Generating neonrtfs_img.c...")

    # 生成 .c 文件的命令
    command = f"{XXD_CMD} -i {NEONRTFS_BIN} > {NEONRTFS_C}"
    result = os.system(command)
    if result != 0:
        print("Failed to generate neonrtfs_img.c")
        return

    print("Generated neonrtfs_img.c successfully!")

    # 新的變數名稱
    new_name = "neonrtfs_img"

    # 讀取生成的 .c 文件
    with open(NEONRTFS_C, "r", encoding="utf-8") as f:
        content = f.read()

    # 使用正則表達式僅替換變數名稱，保留文件內容
    content = re.sub(
        r"unsigned char \w+\[\]",
        f"const unsigned char {new_name}[]",
        content
    )
    content = re.sub(
        r"unsigned int \w+_len",
        f"const unsigned int {new_name}_len",
        content
    )

    # 將修改後的內容寫回 .c 文件
    with open(NEONRTFS_C, "w", encoding="utf-8") as f:
        f.write(content)
   
# 执行步骤
if not os.path.exists(BUILD_DIR):
    os.makedirs(BUILD_DIR)

generate_neonrtfs_bin()
generate_neonrtfs_c()
