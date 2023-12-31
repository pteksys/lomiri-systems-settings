pkg_search_module(GD3 REQUIRED gnome-desktop-3.0)
include_directories(${GD3_INCLUDE_DIRS} ${GLIB_INCLUDE_DIRS} ${ACCOUNTSSERVICE_INCLUDE_DIRS} ${ICU_INCLUDE_DIRS})
add_definitions(-DQT_NO_KEYWORDS)

set(QML_SOURCES
    DisplayLanguage.qml
    KeyboardLayoutItem.qml
    KeyboardLayouts.qml
    PageComponent.qml
    PageHardwareKeyboard.qml
    RebootNecessary.qml
    SpellChecking.qml
    SubsetView.qml
    ThemeValues.qml
)

add_library(LomiriLanguagePlugin MODULE
 keyboard-layout.cpp language-plugin.cpp plugin.cpp subset-model.cpp onscreenkeyboard-plugin.cpp hardwarekeyboard-plugin.cpp
 keyboard-layout.h language-plugin.h plugin.h subset-model.h onscreenkeyboard-plugin.h hardwarekeyboard-plugin.h
 sessionservice.h sessionservice.cpp
 ${QML_SOURCES})
target_link_libraries(LomiriLanguagePlugin Qt5::Qml Qt5::Quick Qt5::DBus LomiriSystemSettingsPrivate ${GD3_LDFLAGS} ${GLIB_LDFLAGS} ${GIO_LDFLAGS} ${ACCOUNTSSERVICE_LDFLAGS} ${ICU_LDFLAGS})

# Would love to get this from lomiri-keyboard directly, but that's QMake .pri,
# so let's just hardcode here.
target_compile_definitions(LomiriLanguagePlugin PRIVATE
    -DLOMIRI_KEYBOARD_PLUGIN_PATH=\"${CMAKE_INSTALL_PREFIX}/lib/lomiri-keyboard/plugins\"
    GETTEXT_PACKAGE="${GETTEXT_PACKAGE}")

set(PLUG_DIR ${PLUGIN_PRIVATE_MODULE_DIR}/Lomiri/SystemSettings/LanguagePlugin)
install(TARGETS LomiriLanguagePlugin DESTINATION ${PLUG_DIR})
install(FILES qmldir DESTINATION ${PLUG_DIR})
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/language)
install(FILES settings-language.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)
install(FILES language.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
