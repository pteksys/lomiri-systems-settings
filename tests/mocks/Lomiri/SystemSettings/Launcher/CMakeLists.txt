find_package(Qt5Qml REQUIRED)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/plugins/launcher
    ${Qt5Qml_INCLUDE_DIRS}
)

set(MOCK_LAUNCHER_SRCS
    ${CMAKE_SOURCE_DIR}/plugins/launcher/launcher.h
    MockLauncherPanelPlugin.cpp
    plugin.cpp
)

add_library(MockLauncherPanelPlugin SHARED ${MOCK_LAUNCHER_SRCS})

target_link_libraries(MockLauncherPanelPlugin Qt5::Qml)

add_lss_mock(Lomiri.SystemSettings.Launcher 1.0 Lomiri/SystemSettings/Launcher
             TARGETS MockLauncherPanelPlugin)
