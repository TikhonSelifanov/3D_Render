cmake_minimum_required(VERSION 3.5)

project(3D_Render LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(3D_Render main.cpp
    camera.cpp
    camera.h
    geometry.cpp
    geometry.h
    objects.cpp
    objects.h
    renderer.cpp
    renderer.h
    box.h
    box.cpp)

include(GNUInstallDirs)
install(TARGETS 3D_Render
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

include(FetchContent)
set(BUILD_SHARED_LIBS OFF)
FetchContent_Declare(SFML GIT_REPOSITORY https://github.com/SFML/SFML.git GIT_TAG 2.6.1)
FetchContent_MakeAvailable(SFML)

FetchContent_Declare(eigen URL https://gitlab.com/libeigen/eigen/-/archive/3.4.0/eigen-3.4.0.zip)
FetchContent_MakeAvailable(eigen)

target_link_libraries(3D_Render eigen)
target_link_libraries(3D_Render sfml-graphics sfml-system sfml-window sfml-audio sfml-network)
