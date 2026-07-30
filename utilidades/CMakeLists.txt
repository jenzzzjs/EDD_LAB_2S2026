cmake_minimum_required(VERSION 3.16)
project(PruebaQt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

add_executable(PruebaQt
    ../Semana_2/vista.cpp
    ../Semana_2/sistema.cpp
    ../Semana_2/listaenlazada.cpp
)
target_include_directories(PruebaQt PRIVATE ../Semana_2)
target_link_libraries(PruebaQt PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)
