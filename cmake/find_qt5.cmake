# 启用Qt的自动moc和uic
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(QT_VERSION_MAJOR 5) # 假设我们使用Qt5
set(QT_REQUIRED_COMPONENTS Core Gui Widgets) # 需要的组件

find_package(Qt${QT_VERSION_MAJOR} REQUIRED
COMPONENTS
    ${QT_REQUIRED_COMPONENTS}
OPTIONAL_COMPONENTS
    Network
)

if(NOT Qt${QT_VERSION_MAJOR}Network_FOUND)
    message("   Qt${QT_VERSION_MAJOR}::Network module not found, crash reporting will be disabled.")
endif()
