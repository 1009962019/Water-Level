project(template_main)

set(GUI_SOURCES )

add_subdirectory(pnc_map)
add_subdirectory(process)
add_subdirectory(ui)

add_executable(${PROJECT_NAME} main.cpp ${GUI_SOURCES})

target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${Process_Dir}
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    process
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Core
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE TRUE
)