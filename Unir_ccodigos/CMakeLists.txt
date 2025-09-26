cmake_minimum_required(VERSION 3.5)

project(Unir_ccodigos LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Unir_ccodigos main.cpp)

include(GNUInstallDirs)
install(TARGETS Unir_ccodigos
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
