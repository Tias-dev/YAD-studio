cmake_minimum_required(VERSION 3.16)

if(NOT CMAKE_PREFIX_PATH AND DEFINED ENV{QTDIR})
  list(APPEND CMAKE_PREFIX_PATH "$ENV{QTDIR}")
endif()

if(APPLE AND NOT CMAKE_PREFIX_PATH)
  set(_qt_mac_candidates
      "$ENV{HOME}/Qt/6.5.3/macos"
      "$ENV{HOME}/Qt/6.5.3/clang_64"
      "/opt/homebrew/opt/qtbase"
      "/opt/homebrew/opt/qt"
      "/usr/local/opt/qtbase"
      "/usr/local/opt/qt")
  foreach(_qt_path IN LISTS _qt_mac_candidates)
    if(EXISTS "${_qt_path}/lib/cmake/Qt6/Qt6Config.cmake")
      list(APPEND CMAKE_PREFIX_PATH "${_qt_path}")
      break()
    endif()
  endforeach()
endif()

set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

if(APPLE)
  get_target_property(_qt_core_location Qt6::Core LOCATION)
  if(_qt_core_location)
    get_filename_component(_qt_lib_dir "${_qt_core_location}" DIRECTORY)
    execute_process(
      COMMAND lipo -info "${_qt_lib_dir}/QtCore"
      OUTPUT_VARIABLE _qt_lipo_info
      ERROR_QUIET
      OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(_qt_lipo_info MATCHES "Non-fat file: .* architecture: x86_64"
       AND CMAKE_SYSTEM_PROCESSOR MATCHES "arm64|aarch64"
       AND NOT CMAKE_OSX_ARCHITECTURES)
      set(CMAKE_OSX_ARCHITECTURES
          x86_64
          CACHE STRING "Qt 5.15 macOS kit is x86_64-only; build via Rosetta on Apple Silicon."
                FORCE)
      message(STATUS "Qt is x86_64-only: setting CMAKE_OSX_ARCHITECTURES=x86_64")
    endif()
  endif()
endif()

set(ADDITIONAL_DIRS ${CMAKE_CURRENT_SOURCE_DIR} Logic Managers Widgets)
include_directories(${ADDITIONAL_DIRS})

file(GLOB_RECURSE YAD_SOURCES LIST_DIRECTORIES FALSE *.cpp)
list(FILTER YAD_SOURCES EXCLUDE REGEX "/(main|headless_main)\\.cpp$")
list(FILTER YAD_SOURCES EXCLUDE REGEX "/moc_.*\\.cpp$")
list(FILTER YAD_SOURCES EXCLUDE REGEX "/qrc_.*\\.cpp$")
list(FILTER YAD_SOURCES EXCLUDE REGEX "_plugin_import\\.cpp$")

add_library(yad_core STATIC ${YAD_SOURCES} Resources.qrc)
target_include_directories(yad_core PUBLIC ${ADDITIONAL_DIRS})
target_link_libraries(
  yad_core
  PUBLIC Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Gui
         Qt${QT_VERSION_MAJOR}::Widgets)

if(MSVC)
  target_compile_options(yad_core PRIVATE /utf-8)
endif()

qt_add_executable(${PROJECT_NAME} main.cpp MANUAL_FINALIZATION)
target_link_libraries(${PROJECT_NAME} PRIVATE yad_core)
target_compile_definitions(${PROJECT_NAME}
    PRIVATE YAD_VERSION="${PROJECT_VERSION}")

# TODO: Add headless version
# add_executable(${PROJECT_NAME}-headless headless_main.cpp)
# target_link_libraries(${PROJECT_NAME}-headless PRIVATE yad_core)
# target_compile_definitions(${PROJECT_NAME}-headless
#                            PRIVATE YAD_VERSION="${PROJECT_VERSION}")

if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)
endif()

if(APPLE AND NOT ANDROID)
  set(_yad_icon "${CMAKE_CURRENT_SOURCE_DIR}/Icons/yad-icon.ico")
  set_source_files_properties(${_yad_icon}
                              PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
  target_sources(${PROJECT_NAME} PRIVATE ${_yad_icon})
  set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES MACOSX_BUNDLE ON
    MACOSX_BUNDLE_BUNDLE_NAME "YAD studio"
               MACOSX_BUNDLE_GUI_IDENTIFIER "ru.mai.yad"
               MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
               MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
               MACOSX_BUNDLE_ICON_FILE "app.icns")
endif()

if(QT_VERSION_MAJOR EQUAL 6)
  qt_finalize_executable(${PROJECT_NAME})
endif()

install(
  TARGETS ${PROJECT_NAME}
  BUNDLE DESTINATION .
  RUNTIME DESTINATION bin)
# TODO Add headless version
# install(TARGETS ${PROJECT_NAME}-headless RUNTIME DESTINATION bin)
install(TARGETS yad_core ARCHIVE DESTINATION lib)
