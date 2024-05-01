project(Water_Level_Display)

set(GUI_SOURCES )

add_subdirectory(image_capture)
add_subdirectory(model_inference)
add_subdirectory(image_process)
add_subdirectory(pnc_map)
add_subdirectory(process)
add_subdirectory(ui)

add_executable(${PROJECT_NAME} main.cpp ${GUI_SOURCES})

target_include_directories(${PROJECT_NAME}
    PRIVATE
    ${PLOG_Dir}
    ${OpenCV_INCLUDE_DIRS}
    ${concurrentqueue_Dir}
    ${ModelInference_Dir}
    ${Src_Dir}
    ${UI_Dir}

)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Core
    ${OpenCV_LIBS}
    image_capture
    image_process
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE TRUE
)