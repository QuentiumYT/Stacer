cmake_minimum_required(VERSION 3.16)
project(Stacer VERSION 1.8.0 LANGUAGES CXX)

# Build with PIC and PIE (default: ON)
option(CMAKE_POSITION_INDEPENDENT_CODE "Determines whether position independent executables or shared libraries will be created" ON)

# Build with LTO (default: OFF)
option(CMAKE_INTERPROCEDURAL_OPTIMIZATION "If set to true, enables interprocedural optimizations if they are known to be supported by the compiler" OFF)

# Apply PIE flags
if(POLICY CMP0083)
  cmake_policy(SET CMP0083 NEW)
  include(CheckPIESupported)
  check_pie_supported()
  set(CMAKE_POLICY_DEFAULT_CMP0083 NEW)
endif()

set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

# Activating MOC and searching for the Qt dependencies
set(CMAKE_AUTOMOC ON)
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Gui Widgets Charts Svg Concurrent Network REQUIRED)

# Setting the minimum C++ standard and passing the Qt-specific define
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_definitions(-DQT_DEPRECATED_WARNINGS)

# Subprojects
add_subdirectory(stacer-core)
add_subdirectory(stacer)

string(TIMESTAMP TODAY_ISO_8601 "%Y-%m-%d" UTC)

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/desktop/stacer.metainfo.xml.in"
  "${CMAKE_CURRENT_BINARY_DIR}/desktop/fr.quentium.stacer.metainfo.xml"
  IMMEDIATE @ONLY
)

install(
  FILES "${CMAKE_CURRENT_BINARY_DIR}/desktop/fr.quentium.stacer.metainfo.xml"
  DESTINATION share/metainfo
)

install(
  FILES "${CMAKE_CURRENT_SOURCE_DIR}/desktop/stacer.desktop"
  DESTINATION share/applications
)

install(
  DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/icons/hicolor"
  DESTINATION share/icons
)

# For Flathub, Stacer is named Acters
option(FLATHUB_BUILD "Install the app as Acters for Flathub" OFF)

if(FLATHUB_BUILD)
  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/desktop/acters.metainfo.xml.in"
    "${CMAKE_CURRENT_BINARY_DIR}/desktop/fr.quentium.acters.metainfo.xml"
    IMMEDIATE @ONLY
  )

  install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/desktop/fr.quentium.acters.metainfo.xml"
    DESTINATION share/metainfo
  )

  install(
    FILES "${CMAKE_CURRENT_SOURCE_DIR}/desktop/acters.desktop"
    DESTINATION share/applications
    RENAME fr.quentium.acters.desktop
  )

  install(
    FILES "${CMAKE_CURRENT_SOURCE_DIR}/icons/hicolor/scalable/apps/stacer.svg"
    DESTINATION share/icons/hicolor/scalable/apps
    RENAME fr.quentium.acters.svg
  )
endif()
