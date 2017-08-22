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

add_subdirectory(ft2232h-spi)
