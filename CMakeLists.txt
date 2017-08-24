project(ft2232h-spi)

cmake_minimum_required(VERSION 3.5)

set(PACKAGE_VERSION 0.1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

include_directories(${CMAKE_SOURCE_DIR})

find_package(FTDI)
if (LIBFTDI_FOUND)
	include_directories(${LIBFTDI_INCLUDE_DIR})
	link_directories(${LIBFTDI_LIBRARIES})
else()
    find_package(MPSSE_SPI REQUIRED)
	include_directories(${LIBMPSSE_SPI_INCLUDE_DIR})
	link_directories(${LIBMPSSE_SPI_LIBRARIES})
endif()

find_package(
    Boost
    1.62.0
    COMPONENTS unit_test_framework
)

add_subdirectory(ft2232h-spi)

if (Boost_UNIT_TEST_FRAMEWORK_FOUND)
    enable_testing()
    add_subdirectory(ft2232h-spi-tests)
else ()
    message(
        WARNING
        "Boost unit test framework not found. Unit tests will not be built."
    )
endif ()
