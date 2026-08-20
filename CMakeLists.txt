cmake_minimum_required(VERSION 3.5..3.20)

project(clowncomptool VERSION 1.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check https://doc.qt.io/qt/deployment-android.html for more information.
# They need to be set before the find_package( ...) calls below.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

set(PROJECT_SOURCES
	"assets/ClownCompTool.rc"
    "icon.qrc"
    "main.cpp"
    "compression.cpp" "compression.h"
    "mainwindow.cpp" "mainwindow.h" "mainwindow.ui"
    "utilities.cpp" "utilities.h"
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(clowncomptool
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(clowncomptool SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(clowncomptool WIN32
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(clowncomptool PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

if(WIN32 AND USE_STATIC_QT_BY_DEFAULT)
    target_link_libraries(clowncomptool PRIVATE
        Qt${QT_VERSION_MAJOR}::QWindowsIntegrationPlugin
        Qt${QT_VERSION_MAJOR}::QWindowsVistaStylePlugin
    )
endif()

set_target_properties(clowncomptool PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER clowncomptool.clownacy.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(clowncomptool)
endif()

add_subdirectory("FW-KENSC-ShellExt" EXCLUDE_FROM_ALL)
target_link_libraries(clowncomptool PRIVATE FW-KENSC-ShellExt-MDComp)

################
# Installation #
################

install(TARGETS clowncomptool DESTINATION "bin")
install(FILES "assets/com.clownacy.ClownCompTool.appdata.xml" DESTINATION "share/metainfo")
install(FILES "assets/com.clownacy.ClownCompTool.desktop" DESTINATION "share/applications")
install(FILES "assets/logo.svg" DESTINATION "share/icons/hicolor/scalable/apps" RENAME "ClownCompTool.svg")
