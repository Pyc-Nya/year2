cmake_minimum_required(VERSION 3.10)
project(Lab02_Classes)

add_executable(${PROJECT_NAME} main.cpp lib_struct.hpp)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
# Включить режим отладки
set(CMAKE_BUILD_TYPE Debug)

# Включить вывод подробной информации о процессе сборки
set(CMAKE_VERBOSE_MAKEFILE ON)

# Включить цветной вывод информации о процессе сборки
set(CMAKE_COLOR_MAKEFILE ON)