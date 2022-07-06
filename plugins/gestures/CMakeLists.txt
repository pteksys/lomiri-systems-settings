add_subdirectory(plugin)

set(QML_SOURCES PageComponent.qml)

add_library(UbuntuGesturesPanel MODULE
  gestures_dbushelper.cpp
  gestures_dbushelper.h
  plugin.cpp
  plugin.h
  ${QML_SOURCES}
)
target_link_libraries(UbuntuGesturesPanel Qt5::Qml Qt5::Quick Qt5::DBus)

set(PLUG_DIR ${PLUGIN_PRIVATE_MODULE_DIR}/Ubuntu/SystemSettings/Gestures)
install(TARGETS UbuntuGesturesPanel DESTINATION ${PLUG_DIR})
install(FILES qmldir.in DESTINATION ${PLUG_DIR} RENAME qmldir)
install(FILES gestures.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/gestures)
