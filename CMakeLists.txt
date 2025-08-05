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
    Examples.hpp

    BicycleImpl.cpp
    Examples.cpp)

include(GNUInstallDirs)
install(TARGETS MyBicycles
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
