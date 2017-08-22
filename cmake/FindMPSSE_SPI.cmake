if (LIBMPSSE_SPI_INCLUDE_DIR AND LIBD2XX_LIBRARIES)
  set (LIBMPSSE_SPI_FOUND TRUE)
else ()
  find_path(LIBMPSSE_SPI_INCLUDE_DIR NAMES MPSSE_SPI.h ftd2xx.h)
  find_library(LIBMPSSE_SPI_LIBRARIES NAMES MPSSE_SPI)
  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(
      LIBMPSSE_SPI DEFAULT_MSG LIBMPSSE_SPI_LIBRARIES LIBMPSSE_SPI_INCLUDE_DIR
  )
endif()

# Shoutout to cmake for making my life a living hell.
if (LIBMPSSE_SPI_FOUND AND MSVC_VERSION GREATER 1800)
  set(CMAKE_C_STANDARD_LIBRARIES 
    "${CMAKE_C_STANDARD_LIBRARIES} legacy_stdio_definitions.lib"
    CACHE STRING "" FORCE
  )
  set(CMAKE_CXX_STANDARD_LIBRARIES 
    "${CMAKE_CXX_STANDARD_LIBRARIES} legacy_stdio_definitions.lib"
    CACHE STRING "" FORCE
  )
endif()
