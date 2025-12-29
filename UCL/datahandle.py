# 输入文件和输出文件的路径
input_file_path = 'sorted.txt'  # 替换为你的输入文件路径
output_file_path = 'sorted1.txt'  # 替换为你希望输出的文件路径

# 打开输入文件并按行读取数据
with open(input_file_path, 'r') as input_file:
    lines = input_file.readlines()

# 处理每一行数据并准备写入新文件
result_lines = []
for line in lines:
    line = line.strip()  # 去除每行两端的空白符（包括换行符）
    if line:  # 确保行不为空
        spaced_line = ' '.join(line[i:i + 1] for i in range(0, len(line), 1))  # 在每8个字符之间加一个空格
        result_lines.append(spaced_line + '\n')  # 添加换行符以保持原文件格式

# 将处理后的结果写入到输出文件
with open(output_file_path, 'w') as output_file:
    output_file.writelines(result_lines)

print(f"文件已成功处理并保存到 {output_file_path}")
