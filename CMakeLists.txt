cmake_minimum_required(VERSION 3.10)

project(HomeWork)

# 查找opencv库
find_package(OpenCV REQUIRED)
# 包含opencv头文件目录
include_directories(${OpenCV_INCLUDE_DIRS})
set(EXECUTABLE_OUTPUT_PATH ../bin)
# 添加源文件
add_executable(main main.cpp)
# 链接opencv库
target_link_libraries(main ${OpenCV_LIBS})



