cmake_minimum_required(VERSION 3.16)

project(MyBicycles LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(MyBicycles main.cpp
    SharedPtr.hpp
    UniquePtr.hpp
    Bicycle.hpp
    BicycleImpl.hpp
    Deleter.hpp
    Examples/UniquePtr_Example.hpp
    Examples/Allocator_Example.hpp

    BicycleImpl.cpp
    Examples/UniquePtr_Example.cpp
    Examples/Allocator_Example.cpp)

target_include_directories(MyBicycles PRIVATE .)

include(GNUInstallDirs)
install(TARGETS MyBicycles
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
