project(MyBicyclesTest LANGUAGES CXX)

enable_testing()

if ($ENV{GOOGLETEST_DIR})
    get_filename_component(_GTEST_BASE $ENV{GOOGLETEST_DIR} REALPATH)
elseif (EXISTS "") # set by QC
    get_filename_component(_GTEST_BASE "" REALPATH) # set by QC
endif()

if (NOT GTEST_ROOT)
    if (_GTEST_BASE)
        message("Setting GTEST_ROOT to ${_GTEST_BASE}")
        set(GTEST_ROOT ${_GTEST_BASE})
    else()
        message("No GTEST_ROOT specified - using system defaults.")
    endif()
endif()

find_package(Threads REQUIRED)

find_package(GTest REQUIRED)
if (NOT GTest_FOUND)
    message (FATAL_ERROR "No GTest Found")
endif()

add_executable(MyBicyclesTest
    main.cpp
    MockBicycle.hpp
    tst_UniquePtr.cpp
    tst_SharedPtr.cpp
    tst_WeakPtr.cpp)

target_include_directories(MyBicyclesTest PRIVATE ..)
add_test(NAME MyBicyclesTest COMMAND MyBicyclesTest)

target_link_libraries(MyBicyclesTest PRIVATE GTest::GTest gmock Threads::Threads)
if (GMock_FOUND)
    target_link_libraries(MyBicyclesTest INTERFACE GTest::GMock)
endif()
