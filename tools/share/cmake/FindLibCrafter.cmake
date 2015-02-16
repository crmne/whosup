# - Try to find LibCrafter
# Once done this will define
#  LIBCRAFTER_FOUND - System has LibCrafter
#  LIBCRAFTER_INCLUDE_DIRS - The LibCrafter include directories
#  LIBCRAFTER_LIBRARIES - The libraries needed to use LibXml2
#  LIBCRAFTER_DEFINITIONS - Compiler switches required for using LibXml2

find_package(PkgConfig)
pkg_check_modules(PC_LIBCRAFTER QUIET libcrafter)
set(LIBCRAFTER_DEFINITIONS ${PC_LIBCRAFTER_CFLAGS_OTHER})

find_path(LIBCRAFTER_INCLUDE_DIR crafter.h
          HINTS ${PC_LIBCRAFTER_INCLUDEDIR} ${PC_LIBCRAFTER_INCLUDE_DIRS})

find_library(LIBCRAFTER_LIBRARY NAMES crafter libcrafter
             HINTS ${PC_LIBCRAFTER_LIBDIR} ${PC_LIBCRAFTER_LIBRARY_DIRS} )

set(LIBCRAFTER_LIBRARIES ${LIBCRAFTER_LIBRARY} )
set(LIBCRAFTER_INCLUDE_DIRS ${LIBCRAFTER_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBCRAFTER_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(LibCrafter  DEFAULT_MSG
                                  LIBCRAFTER_LIBRARY LIBCRAFTER_INCLUDE_DIR)

mark_as_advanced(LIBCRAFTER_INCLUDE_DIR LIBCRAFTER_LIBRARY )