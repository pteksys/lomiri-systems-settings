include_directories(
    ../src
    ${CMAKE_CURRENT_BINARY_DIR}
    ${GLIB_INCLUDE_DIRS}
    ${QTDBUSMOCK_INCLUDE_DIRS}
    ${QTDBUSTEST_INCLUDE_DIRS}
)

set(XVFB_CMD xvfb-run -a -s "-screen 0 640x480x24")

add_definitions(-DI18N_DOMAIN="lomiri-system-settings")
add_definitions(-DPLUGIN_PRIVATE_MODULE_DIR="${PLUGIN_PRIVATE_MODULE_DIR}")
add_definitions(-DPLUGIN_MODULE_DIR="${CMAKE_CURRENT_BINARY_DIR}")
add_definitions(-DMANIFEST_DIR="data")
add_definitions(-DPLUGIN_MANIFEST_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data")
add_definitions(-DQML_TEST_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
add_definitions(-DQML_DIR="${CMAKE_CURRENT_BINARY_DIR}")
add_definitions(-DPLUGIN_QML_DIR="${CMAKE_CURRENT_BINARY_DIR}")
add_definitions(-DSYSTEM_IMAGE_DBUS_TEMPLATE="${CMAKE_SOURCE_DIR}/tests/autopilot/lomiri_system_settings/tests/systemimage.py")

add_library(test-plugin SHARED test-plugin.cpp test-plugin.h)
target_link_librarieS(test-plugin LomiriSystemSettings Qt5::Core Qt5::Qml)

add_library(test-plugin2 SHARED test-plugin2.cpp test-plugin2.h)
target_link_librarieS(test-plugin2 LomiriSystemSettings Qt5::Core Qt5::Qml)

add_executable(tst-plugins
    tst_plugins.cpp
    ../src/debug.cpp
    ../src/item-model.cpp
    ../src/plugin-manager.cpp
    ../src/plugin.cpp
    ../src/debug.h
    ../src/item-model.h
    ../src/plugin-manager.h
    ../src/plugin.h
)

add_executable(tst-arguments
    tst_arguments.cpp
    ../src/utils.cpp
)

target_link_libraries(tst-plugins Qt5::Core Qt5::Qml Qt5::Test LomiriSystemSettings ${GLIB_LDFLAGS})
add_test(tst-plugins tst-plugins)
set_tests_properties(tst-plugins PROPERTIES ENVIRONMENT
    "QT_QPA_PLATFORM=minimal;XDG_DATA_DIRS=${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(tst-arguments Qt5::Core Qt5::Test ${GLIB_LDFLAGS})
add_test(tst-arguments tst-arguments)

add_executable(tst-systemimage
    tst_systemimage.cpp
    mocks/system-image-dbus/fakesystemimagedbus.cpp
)
add_test(tst-systemimage tst-systemimage)
target_link_libraries(tst-systemimage
    Qt5::Core Qt5::DBus Qt5::Test
    LomiriSystemSettingsPrivate
    ${QTDBUSMOCK_LIBRARIES}
    ${QTDBUSTEST_LIBRARIES}
)

add_subdirectory(utils)

# QML tests that require graphical capabilities.
add_custom_target(qmluitests)
add_dependencies(qmluitests LomiriSettingsTest)

add_custom_target(qmltests)
add_dependencies(qmltests qmluitests)

add_subdirectory(mocks)
add_subdirectory(plugins)
