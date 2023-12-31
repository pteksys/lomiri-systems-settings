add_subdirectory(Components)

set(QML_SOURCES
    MainPage.qml
    WallpaperGrid.qml
    Preview.qml
    utilities.js
)
add_library(LomiriBackgroundPanel MODULE
plugin.cpp background.cpp plugin.h background.h
${QML_SOURCES}) # So they show up in Qt designer.

target_link_libraries(LomiriBackgroundPanel Qt5::Qml Qt5::Quick Qt5::DBus LomiriSystemSettingsPrivate)

set(PLUG_DIR ${PLUGIN_PRIVATE_MODULE_DIR}/Lomiri/SystemSettings/Background)
install(TARGETS LomiriBackgroundPanel DESTINATION ${PLUG_DIR})
install(FILES qmldir DESTINATION ${PLUG_DIR})
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/background)
install(FILES welcomeoverlay.svg import-image@18.png header_handlearrow.png header_handlearrow2.png bullet.png
        DESTINATION ${PLUGIN_QML_DIR}/background)
install(FILES settings-background.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)
install(FILES background.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
