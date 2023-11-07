include_directories(${GLIB_INCLUDE_DIRS})

add_definitions(-DI18N_DIRECTORY="${CMAKE_INSTALL_PREFIX}/share/locale")
add_definitions(-DI18N_DOMAIN="lomiri-system-settings")
add_definitions(-DPLUGIN_PRIVATE_MODULE_DIR="${PLUGIN_PRIVATE_MODULE_DIR}")
add_definitions(-DMANIFEST_DIR="${MANIFEST_DIR}")
add_definitions(-DPLUGIN_MANIFEST_DIR="${PLUGIN_MANIFEST_DIR}")
add_definitions(-DQML_DIR="${QML_DIR}")
add_definitions(-DPLUGIN_QML_DIR="${PLUGIN_QML_DIR}")
add_definitions(-DPLUGIN_MODULE_DIR="${PLUGIN_MODULE_DIR}")

add_subdirectory(SystemSettings)

set(USS_SOURCES
    debug.cpp
    item-model.cpp
    main.cpp
    plugin-manager.cpp
    plugin.cpp
    utils.cpp
)

set(QML_SOURCES
    qml/AdaptiveContainer.qml
    qml/CategorySection.qml
    qml/EntryComponent.qml
    qml/EntryComponentGrid.qml
    qml/EntryComponentList.qml
    qml/MainWindow.qml
    qml/UncategorizedItemsView.qml
    qml/magnifyingglass.svg
    SystemSettings/ItemPage.qml
    SystemSettings/SettingsItemTitle.qml
)

QT5_ADD_RESOURCES(system-settings-resources ui.qrc)

add_executable(lomiri-system-settings ${USS_SOURCES} ${QML_SOURCES} ${system-settings-resources})
target_link_libraries(lomiri-system-settings Qt5::Core Qt5::Gui Qt5::Quick Qt5::Qml Qt5::DBus Qt5::Widgets LomiriSystemSettings LomiriSystemSettingsPrivate ${GLIB_LDFLAGS})
install(TARGETS lomiri-system-settings RUNTIME DESTINATION bin)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/url-map.ini DESTINATION ${PLUGIN_MANIFEST_DIR})
