cmake_minimum_required(VERSION 3.12)
project(pomodoro)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_VERBOSE_MAKEFILE ON)
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -fno-strict-aliasing -fno-omit-frame-pointer -g -O0 -ggdb")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fno-strict-aliasing -fno-omit-frame-pointer -g -O0 -ggdb")

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets)

set(WIN32_FLAG "")
if(MSVC)
    set(WIN32_FLAG WIN32)
endif()

add_executable(pomodoro ${WIN32_FLAG}
    main.cpp 
    widget.cpp
    message_box.cpp
)

target_link_libraries(pomodoro Qt${QT_VERSION_MAJOR}::Widgets)
if(PRODUCTION_OPTIMIZATION)
    if(MSVC)
        target_compile_options(pomodoro PRIVATE /O2 /Ob2 /Oi /Ot)
        message(STATUS "已为 MSVC 启用生产优化")
    else()
        target_compile_options(pomodoro PRIVATE -O3 -march=native)
        message(STATUS "已为非 MSVC 启用生产优化")
    endif()
else()
    if(MSVC)
        target_compile_options(pomodoro PRIVATE /Od /Zi)
    else()
        target_compile_options(pomodoro PRIVATE -O0 -g)
    endif()
    message(STATUS "调试友好构建（未启用生产优化）")
endif()
