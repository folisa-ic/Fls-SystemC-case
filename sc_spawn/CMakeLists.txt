# 设置 CMake 的最低版本要求
cmake_minimum_required(VERSION 3.10)

# 定义工程的名字
project(sp)

# 设置编译器为 g++
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)  # 确保必须使用 C++14
set(CMAKE_CXX_EXTENSIONS OFF)          # 禁用编译器特定的扩展

# 生成 compile_commands.json 文件
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 指定额外的头文件路径
include_directories($ENV{SYSTEMC_HOME}/include/)

# 指定额外的库文件路径
link_directories($ENV{SYSTEMC_HOME}/lib-linux64/)

# 将源文件添加到工程中
add_executable(sp sp.cc)

# 链接所需的动态库
target_link_libraries(sp libsystemc.so)
