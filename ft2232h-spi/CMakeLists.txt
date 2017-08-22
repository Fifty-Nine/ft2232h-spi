cmake_minimum_required(VERSION 3.5)

project(ft2232h-spi)

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

install(
    TARGETS ft2232h-spi
    EXPORT ft2232h-spi
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include
    PRIVATE_HEADER DESTINATION include/ft2232h-spi
)
install(EXPORT ft2232h-spi NAMESPACE ft2232h_spi_ DESTINATION share/ft2232h-spi)

if (LIBFTDI_FOUND)
    target_link_libraries(ft2232h-spi ftdi)
else()
	target_link_libraries(ft2232h-spi libmpsse)
endif()

set(ft2232h-spi_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}
    CACHE INTERNAL "ft2232h-spi: Include Directories" FORCE)
