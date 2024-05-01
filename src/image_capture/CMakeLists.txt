project(image_capture)

add_library(${PROJECT_NAME}
    SHARED
    image_capture_thread.cpp
    rtsp_camera.cpp
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    ${OpenCV_LIBS}
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
    ${PLOG_Dir}
    ${OpenCV_INCLUDE_DIRS}
    ${concurrentqueue_Dir}
)

message(${concurrentqueue_Dir})