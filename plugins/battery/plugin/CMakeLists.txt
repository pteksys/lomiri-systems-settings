include_directories(${CMAKE_CURRENT_BINARY_DIR})

include_directories(${GLIB_INCLUDE_DIRS} ${UPOWER_GLIB_INCLUDE_DIRS})

add_definitions(-DQT_NO_KEYWORDS)


add_library(battery-plugin SHARED battery-plugin.h battery-plugin.cpp)
target_link_libraries(battery-plugin Qt5::Core Qt5::Qml LomiriSystemSettings ${GLIB_LDFLAGS} ${UPOWER_GLIB_LDFLAGS})
install(TARGETS battery-plugin DESTINATION ${PLUGIN_MODULE_DIR})
