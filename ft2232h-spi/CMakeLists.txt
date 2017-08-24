cmake_minimum_required(VERSION 3.5)

set(ft2232h-spi_PUBLIC_HEADERS
    ft2232h-spi.h
)

set(ft2232h-spi_PRIVATE_HEADERS
    exceptions.h
    packet.h
    packet-detail.h
    util.h
)


set(ft2232h-spi_SOURCE_FILES
    packet.cpp
    ${version_src_file}
)

if (LIBFTDI_FOUND)
    set(ft2232h-spi_SOURCE_FILES
        ${ft2232h-spi_SOURCE_FILES}
        ft2232h-spi-libftdi.cpp
    )
else()
    find_package(MPSSE_SPI REQUIRED)
    set(ft2232h-spi_SOURCE_FILES
        ${ft2232h-spi_SOURCE_FILES}
        ft2232h-spi-libmpsse-spi.cpp
    )
endif()

add_library(ft2232h-spi ${ft2232h-spi_SOURCE_FILES})

set_target_properties(
    ft2232h-spi PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)
set_target_properties(
    ft2232h-spi PROPERTIES
    PUBLIC_HEADER "${ft2232h-spi_PUBLIC_HEADERS}"
)
set_target_properties(
    ft2232h-spi PROPERTIES
    PRIVATE_HEADER "${ft2232h-spi_PRIVATE_HEADERS}"
)
set(ft2232h-spi_INCLUDE_DIR "${CMAKE_INSTALL_PREFIX}/include")

if (LIBFTDI_FOUND)
    target_link_libraries(ft2232h-spi ${LIBFTDI_LIBRARIES})
    set(ft2232h-spi_LIBRARY_DIRS ${LIBFTDI_LIBRARY_DIRS})
else()
    target_link_libraries(ft2232h-spi libmpsse)
    set(ft2232h-spi_LIBRARY_DIRS ${LIBMPSSE_SPI_LIBRARY_DIRS})
endif()

include(CMakePackageConfigHelpers)
set(ConfigPackageLocation "lib/cmake/ft2232h-spi")
install(
    TARGETS ft2232h-spi
    EXPORT ft2232h-spiTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include
    PRIVATE_HEADER DESTINATION include/ft2232h-spi
)
install(EXPORT ft2232h-spiTargets DESTINATION ${ConfigPackageLocation})

configure_package_config_file(
    cmake/ft2232h-spi-config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/ft2232h-spi-config.cmake"
    INSTALL_DESTINATION "${ConfigPackageLocation}"
    PATH_VARS ft2232h-spi_INCLUDE_DIR ft2232h-spi_LIBRARY_DIRS
)


install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/ft2232h-spi-config.cmake"
    DESTINATION
        ${ConfigPackageLocation}
)


