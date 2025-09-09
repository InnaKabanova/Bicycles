cmake_minimum_required(VERSION 3.16)

project(MyBicycles LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(LibBicycles
    Bicycle.hpp
    BicycleImpl.hpp
    BicycleImpl.cpp

    MemoryManagement/Deleter.hpp
    MemoryManagement/MyAllocatorBase.hpp
    MemoryManagement/MyAllocatorOnStack.hpp
    MemoryManagement/MyAllocatorNonOwning.hpp
    MemoryManagement/SimpleSegmentManager.hpp
    MemoryManagement/SimpleSegmentManager.cpp
    MemoryManagement/SharedPtr.hpp
    MemoryManagement/UniquePtr.hpp

    Examples/UniquePtr_Example.hpp
    Examples/UniquePtr_Example.cpp
    Examples/Allocator_Example.hpp
    Examples/Allocator_Example.cpp)
target_include_directories(LibBicycles PUBLIC .)

add_executable(MyBicycles main.cpp)
target_link_libraries(MyBicycles PRIVATE LibBicycles)

install(TARGETS MyBicycles LibBicycles
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

add_subdirectory(UnitTests)
