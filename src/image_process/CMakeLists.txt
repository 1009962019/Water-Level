project(image_process)

add_library(${PROJECT_NAME}
    SHARED
    image_process_thread.cpp
    water_level_process.cpp
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    ${OpenCV_LIBS}
    model_inference
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PLOG_Dir}
    ${OpenCV_INCLUDE_DIRS}
    ${concurrentqueue_Dir}
    ${ModelInference_Dir}
    ${Src_Dir}
)