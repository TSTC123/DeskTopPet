cmake_minimum_required(VERSION 3.16)

project(DesktopPet VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON) # 启用自动UI编译
set(CMAKE_AUTOMOC ON) # 自动处理 Qt 的 moc（元对象编译器）
set(CMAKE_AUTORCC ON) # 自动处理 Qt 的资源文件（.qrc）

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 关键修改：添加 Multimedia 模块
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS
    Widgets
    Multimedia
    MultimediaWidgets
)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS
    Widgets
    Multimedia
    MultimediaWidgets
)
# 在find_package之后添加以下内容，在CMakeLists.txt中启用QML调试
if (WIN32)
    add_definitions(-DQT_QML_DEBUG)
else()
    add_compile_definitions(QT_QML_DEBUG)
endif()

set(PROJECT_SOURCES
        main.cpp
        petwidget.cpp
        petwidget.h
        petstate.cpp
        menuwindow.cpp
        menuwindow.h
        clockwindow.cpp
        clockwindow.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(DesktopPet
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        PetBase.h
        PetAnimation.h


        Image/Image.qrc
        petstate.h petstate.cpp
        menuwindow.h menuwindow.cpp
        clockwindow.h clockwindow.cpp

    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET DesktopPet APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(DesktopPet SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(DesktopPet
            ${PROJECT_SOURCES}
        )
    endif()
endif()

# 关键修改：链接所有需要的Qt模块
target_link_libraries(DesktopPet PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Multimedia
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.DesktopPet)
endif()
set_target_properties(DesktopPet PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS DesktopPet
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(DesktopPet)
endif()
