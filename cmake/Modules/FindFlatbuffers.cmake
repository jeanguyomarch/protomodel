# We first start to search for flatbuffers in the .deps/ directoy.  If we ran
# the install-deps.sh script before, we would find there a hand-compiled
# flatbuffers
find_library(FLATBUFFERS_LIBRARIES flatbuffers
   PATHS "${CMAKE_SOURCE_DIR}/.deps/_sysroot/lib"
   NO_DEFAULT_PATH)

set(FLATBUFFERS_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/.deps/_sysroot/include")

# Validate the package with FLATBUFFERS_LIBRARIES find above
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
   Flatbuffers REQUIRED_VARS FLATBUFFERS_LIBRARIES)

