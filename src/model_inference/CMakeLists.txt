project(model_inference)

find_package(CUDA REQUIRED)
message(STATUS " libraries: ${CUDA_LIBRARIES}")
message(STATUS " include path: ${CUDA_INCLUDE_DIRS}")
enable_language(CUDA) 

add_library(${PROJECT_NAME}
    SHARED
    BaseInference.cpp
    SegmentationInference.cpp
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    ${OpenCV_LIBS}
    nvinfer
    cudart
    nvinfer_plugin
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
    ${OpenCV_INCLUDE_DIRS}
    ${CUDA_INCLUDE_DIRS}
)
