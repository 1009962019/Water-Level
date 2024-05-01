cmake_minimum_required(VERSION 3.25.0)
project(planning
    VERSION 0.0.1
    DESCRIPTION "a demo of cmake planning"
    HOMEPAGE_URL "https://github.com/1009962019/Qt_template"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

# 变量和目录设置
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

set(Src_Dir ${CMAKE_SOURCE_DIR}/src)
set(ModelInference_Dir ${CMAKE_SOURCE_DIR}/src/model_inference)

set(Process_Dir ${CMAKE_SOURCE_DIR}/src/process)
set(PNC_Map_Dir ${CMAKE_SOURCE_DIR}/src/pnc_map)
set(UI_Dir ${CMAKE_SOURCE_DIR}/src/ui)

# 第三方库
include("./cmake/find_qt5.cmake")
include("./cmake/find_concurrentqueue.cmake")
include("./cmake/find_plog.cmake")
find_package(OpenCV REQUIRED)

# 添加子目录
add_subdirectory(src)
add_subdirectory(test/map_test)