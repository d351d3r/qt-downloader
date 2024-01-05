cmake_minimum_required(VERSION 3.5)
project(SberDownloader VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,undefined -fno-omit-frame-pointer")
set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=address,undefined")

add_compile_options(-Wall -Wextra -Werror -Wconversion)
add_compile_definitions(_LIBCPP_ENABLE_THREAD_SAFETY_ANNOTATIONS)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Network Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Network Widgets)

set(PROJECT_SOURCES
        src/main.cpp
        src/mainwindow.cpp
        src/filedownloader.cpp
        src/mainwindowmodel.cpp
        include/mainwindow.h
        include/mainwindowmodel.h
        include/filedownloader.h
        src/mainwindow.ui
)

include_directories(${PROJECT_SOURCE_DIR}/include)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(SberDownloader
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(SberDownloader SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(SberDownloader
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(SberDownloader PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network)

set_target_properties(SberDownloader PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(SberDownloader)
endif()
