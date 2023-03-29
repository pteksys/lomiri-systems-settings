include_directories(${CMAKE_CURRENT_BINARY_DIR} ../)

add_definitions(-DLAUNCHER_PLUGIN_QML_DIR="${PLUGIN_QML_DIR}/launcher" -DQT_NO_KEYWORDS)
add_library(launcher-plugin SHARED launcher-plugin.h launcher-plugin.cpp ../launcher.h ../launcher_impl.cpp)
target_include_directories(launcher-plugin PUBLIC ${GIO_INCLUDE_DIRS})
target_link_libraries(launcher-plugin ${GIO_LIBRARIES} Qt5::Core Qt5::Qml Qt5::Widgets LomiriSystemSettings)
install(TARGETS launcher-plugin DESTINATION ${PLUGIN_MODULE_DIR})
install(FILES GuAccessor.qml DESTINATION ${PLUGIN_QML_DIR}/launcher)
