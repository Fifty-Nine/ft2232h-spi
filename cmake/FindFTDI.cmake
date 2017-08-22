if (LIBFTDI_INCLUDE_DIR AND LIBFTDI_LIBRARIES)
  set (LIBFTDI_FOUND TRUE)
else ()
  find_package(PkgConfig)
  pkg_search_module(PC_LIBFTDI libftdi1 libftdi)

  find_path(LIBFTDI_INCLUDE_DIR ftdi.h
    PATH_SUFFIXES libftdi libftdi1
    PATHS ${PC_LIBFTDI_INCLUDEDIR} ${PC_LIBFTDI_INCLUDE_DIRS}
  )

  find_library(LIBFTDI_LIBRARIES NAMES ftdi1 ftdi
    PATHS ${PC_LIBFTDI_LIBDIR} ${PC_LIBFTDI_LIBRARY_DIRS})

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(LIBFTDI DEFAULT_MSG LIBFTDI_LIBRARIES LIBFTDI_INCLUDE_DIR)
endif()
