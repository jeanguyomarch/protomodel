# We first start to search for mstch in the .deps/ directoy.  If we ran the
# install-deps.sh script before, we would find there a hand-compiled mstch
find_library(MSTCH_LIBRARIES mstch
   PATHS "${CMAKE_SOURCE_DIR}/.deps/_sysroot/lib"
   NO_DEFAULT_PATH)

set(MSTCH_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/.deps/_sysroot/include")

# Validate the package with MSTCH_LIBRARIES find above
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
   Mstch REQUIRED_VARS MSTCH_LIBRARIES)
