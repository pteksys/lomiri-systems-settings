cmake_minimum_required(VERSION 3.5)

project(lomiri-system-settings VERSION 1.0.2 LANGUAGES C CXX)

if(${PROJECT_BINARY_DIR} STREQUAL ${PROJECT_SOURCE_DIR})
   message(FATAL_ERROR "In-tree build attempt detected, aborting. Set your build dir outside your source dir, delete CMakeCache.txt from source root and try again.")
endif()

string(TOLOWER "${CMAKE_BUILD_TYPE}" cmake_build_type_lower) # Build types should always be lowercase but sometimes they are not.

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(EnableCoverageReport)
include(FindPkgConfig)
include(GNUInstallDirs)
set(LIBDIR ${CMAKE_INSTALL_LIBDIR})
enable_testing()

option(ENABLE_TESTS "Build tests" ON)
option(ENABLE_LIBDEVICEINFO "Build with libdeviceinfo support" OFF)

if(cmake_build_type_lower MATCHES coverage)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage" )
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage" )
  set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} --coverage" )
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} --coverage" )

  # We add -g when building with coverage so valgrind reports line numbers.
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g" )
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g" )
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -fno-permissive -pedantic -Wall -Wextra")
find_package(Qt5Concurrent REQUIRED)
find_package(Qt5DBus REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Qml REQUIRED)
find_package(Qt5Quick REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(Qt5Test REQUIRED)
find_package(Qt5Widgets REQUIRED)

# Workaround for https://bugreports.qt-project.org/browse/QTBUG-29987
set(QT_IMPORTS_DIR "${CMAKE_INSTALL_LIBDIR}/qt5/qml")
set(CMAKE_AUTOMOC ON)

pkg_search_module(GOBJECT REQUIRED gobject-2.0)
pkg_search_module(GLIB REQUIRED glib-2.0)
pkg_check_modules(GIO REQUIRED gio-2.0 gio-unix-2.0)
pkg_search_module(UPOWER_GLIB REQUIRED upower-glib)
pkg_search_module(ACCOUNTSSERVICE REQUIRED accountsservice)
pkg_search_module(GEONAMES REQUIRED geonames)
pkg_search_module(ICU REQUIRED icu-i18n)
pkg_search_module(ANDR_PROP libandroid-properties)
pkg_search_module(CLICK REQUIRED click-0.4)
pkg_check_modules(QTGSETTINGS REQUIRED gsettings-qt)
if(ENABLE_LIBDEVICEINFO)
    pkg_check_modules(DEVICEINFO REQUIRED deviceinfo)
endif()

set(DATA_DIR share)
set(MODULE_DIR lomiri-system-settings)
set(MANIFEST_DIR lomiri-system-settings)
set(QML_DIR qml-plugins)
set(PRIVATE_DIR private)

# share/lomiri-system-settings
set(PLUGIN_MANIFEST_DIR_BASE ${DATA_DIR}/${MANIFEST_DIR})
# lomiri-system-settings
set(PLUGIN_MODULE_DIR_BASE ${MODULE_DIR})
# lomiri-system-settings/private
set(PLUGIN_PRIVATE_MODULE_DIR_BASE "${PLUGIN_MODULE_DIR_BASE}/${PRIVATE_DIR}")
# share/lomiri-system-settings/qml-plugins
set(PLUGIN_QML_DIR_BASE ${DATA_DIR}/${MANIFEST_DIR}/${QML_DIR})
# E.g. /usr/share/lomiri-system-settings
set(PLUGIN_MANIFEST_DIR "${CMAKE_INSTALL_PREFIX}/${PLUGIN_MANIFEST_DIR_BASE}")
# E.g. /usr/lib/lomiri-system-settings
set(PLUGIN_MODULE_DIR "${CMAKE_INSTALL_PREFIX}/${LIBDIR}/${PLUGIN_MODULE_DIR_BASE}")
# E.g. /usr/share/lomiri-system-settings/qml-plugins
set(PLUGIN_QML_DIR "${CMAKE_INSTALL_PREFIX}/${PLUGIN_QML_DIR_BASE}")
# E.g. /usr/lib/lomiri-system-settings/private
set(PLUGIN_PRIVATE_MODULE_DIR "${CMAKE_INSTALL_PREFIX}/${LIBDIR}/${PLUGIN_PRIVATE_MODULE_DIR_BASE}")
set(SETTINGS_SHARE_DIR "${CMAKE_INSTALL_PREFIX}/${PLUGIN_MANIFEST_DIR_BASE}")

SET(CMAKE_INSTALL_RPATH "${PLUGIN_MODULE_DIR}")

set(DESKTOP_FILE lomiri-system-settings.desktop)
set(GETTEXT_PACKAGE lomiri-system-settings)
set(DESKTOP_FILE ${PROJECT_NAME}.desktop)

add_subdirectory(po)
add_subdirectory(schema)
add_subdirectory(lib)
include_directories(lib)
include_directories(src)
add_subdirectory(plugins)
add_subdirectory(src)

include(FindGettext)
configure_file(${DESKTOP_FILE}.in.in ${DESKTOP_FILE}.in)
add_custom_target(${DESKTOP_FILE} ALL
    COMMENT "Merging translations into ${DESKTOP_FILE}..."
    COMMAND ${GETTEXT_MSGFMT_EXECUTABLE}
            --desktop --template=${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_FILE}.in
            -o ${DESKTOP_FILE}
            -d ${CMAKE_SOURCE_DIR}/po
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_FILE}
        DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)

install(FILES lomiri-system-settings.url-dispatcher DESTINATION share/lomiri-url-dispatcher/urls)
install(FILES screenshot.png DESTINATION ${SETTINGS_SHARE_DIR})
install(FILES system-settings.svg DESTINATION ${SETTINGS_SHARE_DIR})
install(FILES system-settings-app-splash.svg DESTINATION ${SETTINGS_SHARE_DIR})

if(cmake_build_type_lower MATCHES coverage)
  ENABLE_COVERAGE_REPORT(TARGETS system-settings FILTER /usr/include ${CMAKE_SOURCE_DIR}/tests/* ${CMAKE_BINARY_DIR}/*)
endif()
