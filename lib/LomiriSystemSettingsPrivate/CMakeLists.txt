set(HEADERS
    accountsservice.h
    i18n.h
    systemimage.h
)
set(SOURCES
    accountsservice.cpp
    i18n.cpp
    systemimage.cpp
)

add_library(LomiriSystemSettingsPrivate SHARED ${HEADERS} ${SOURCES})
set_target_properties(LomiriSystemSettingsPrivate PROPERTIES
VERSION 0.0
SOVERSION 0.0
)

target_link_libraries(LomiriSystemSettingsPrivate Qt5::Core Qt5::DBus)

# Makes the life easier for in-tree plugins
target_include_directories(LomiriSystemSettingsPrivate PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

install(TARGETS LomiriSystemSettingsPrivate LIBRARY DESTINATION ${LIBDIR})
install(FILES ${HEADERS}
        DESTINATION include/LomiriSystemSettings/private)
