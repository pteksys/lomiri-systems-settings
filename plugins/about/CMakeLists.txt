include_directories(${ANDR_PROP_INCLUDE_DIRS})
include_directories(${DEVICEINFO_INCLUDE_DIRS})
include_directories(${GLIB_INCLUDE_DIRS})
include_directories(${GIO_INCLUDE_DIRS})
include_directories(${CLICK_INCLUDE_DIRS})
include_directories(${CLICK_INCLUDE_DIRS})
add_definitions(-DQT_NO_KEYWORDS)

if(DEVICEINFO_FOUND)
    add_definitions(-DENABLE_DEVICEINFO)
endif()

set(QML_SOURCES
    EntryComponent.qml
    License.qml
    PageComponent.qml
    SingleValueStacked.qml
    Software.qml
    Storage.qml
    StorageBar.qml
    StorageItem.qml
    Version.qml
    DevMode.qml
    fakepkgslist.xml
)

add_library(LomiriStorageAboutPanel MODULE
    plugin.cpp
    storageabout.cpp
    systeminfo.cpp
    click.cpp
    plugin.h
    storageabout.h
    click.h
    ${QML_SOURCES} # So they show up in Qt designer.
)

target_link_libraries(LomiriStorageAboutPanel Qt5::Qml Qt5::Quick Qt5::DBus
${ANDR_PROP_LDFLAGS} ${DEVICEINFO_LDFLAGS} ${GLIB_LDFLAGS} ${GIO_LDFLAGS} ${CLICK_LDFLAGS} LomiriSystemSettingsPrivate)

set(PLUG_DIR ${PLUGIN_PRIVATE_MODULE_DIR}/Lomiri/SystemSettings/StorageAbout)
install(TARGETS LomiriStorageAboutPanel DESTINATION ${PLUG_DIR})
install(FILES qmldir DESTINATION ${PLUG_DIR})
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/about)
install(FILES settings-about.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)
install(FILES about.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
