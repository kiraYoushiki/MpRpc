#设置cmake最低版本和项目名称
cmake_minimum_required(VERSION 3.0)
project(mprpc)

#设置可执行文件的输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
#设置库文件的输出路径
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
#设置项目编译头文件的搜索路径 -I
include_directories(${PROJECT_SOURCE_DIR}/src/include)
include_directories(${PROJECT_SOURCE_DIR}/example)
#设置项目库文件的搜索路径 -L
link_directories(${PROJECT_SOURCE_DIR}/lib) 

add_subdirectory(src)
add_subdirectory(example)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
