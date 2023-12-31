set(QML_SOURCES
AccessPoint.qml
BaseMenuItem.qml
CertPicker.qml
CertDialog.qml
Common.qml
DivMenuItem.qml
FramedMenuItem.qml
HLine.qml
IndicatorBase.qml
MenuItemFactory.qml
NetworkDetails.qml
NetworkDetailsBrief.qml
OtherNetwork.qml
PageComponent.qml
PreviousNetworks.qml
RemoveBackground.qml
SectionMenuItem.qml
StandardMenuItem.qml
SwitchMenuItem.qml
)

add_library(LomiriWifiPanel MODULE
  certhandler.cpp
  plugin.cpp
  previousnetworkmodel.cpp
  unitymenumodelstack.cpp
  wifidbushelper.cpp
  certhandler.h
  nm_manager_proxy.h
  nm_settings_proxy.h
  nm_settings_connection_proxy.h
  plugin.h
  previousnetworkmodel.h
  unitymenumodelstack.h
  wifidbushelper.h
  ${QML_SOURCES}
)
target_link_libraries(LomiriWifiPanel Qt5::Qml Qt5::Quick Qt5::DBus)

set(PLUG_DIR ${PLUGIN_PRIVATE_MODULE_DIR}/Lomiri/SystemSettings/Wifi)
install(TARGETS LomiriWifiPanel DESTINATION ${PLUG_DIR})
install(FILES qmldir.in DESTINATION ${PLUG_DIR} RENAME qmldir)
install(FILES wifi.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
install(FILES settings-wifi.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/wifi)
