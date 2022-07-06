include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(gestures-plugin SHARED gestures-plugin.h gestures-plugin.cpp ../gestures_dbushelper.cpp)
target_link_libraries(gestures-plugin Qt5::Core Qt5::Qml Qt5::DBus SystemSettings)
install(TARGETS gestures-plugin DESTINATION ${PLUGIN_MODULE_DIR})
