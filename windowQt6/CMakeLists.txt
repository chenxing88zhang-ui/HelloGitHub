cmake_minimum_required(VERSION 3.16)

project(windowQt6 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# 指向 Poco install 路径
set(CMAKE_PREFIX_PATH "C:/Poco/install")

# 查找 Poco 库
find_package(Poco REQUIRED COMPONENTS Foundation Net Util JSON XML)


set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(windowQt6
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET windowQt6 APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(windowQt6 SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(windowQt6
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(windowQt6 PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# 链接 Qt 和 Poco
target_link_libraries(windowQt6 PRIVATE
    Poco::Foundation
    Poco::Net
    Poco::Util
    Poco::JSON
    Poco::XML
)

# 如果是 DLL，构建后复制到可执行目录
add_custom_command(TARGET windowQt6 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "C:/Poco/install/bin/PocoFoundationd.dll"
    "C:/Poco/install/bin/PocoNetd.dll"
    "C:/Poco/install/bin/PocoUtild.dll"
    $<TARGET_FILE_DIR:windowQt6>
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.windowQt6)
endif()
set_target_properties(windowQt6 PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS windowQt6
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(windowQt6)
endif()
