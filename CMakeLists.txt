###############################################################################
# Setup
###############################################################################

cmake_minimum_required(VERSION 3.0.2)
project(protomodel LANGUAGES CXX VERSION 0.1.0)
set(CMAKE_MODULE_PATH
   "${CMAKE_MODULE_PATH}${CMAKE_SOURCE_DIR}/cmake/Modules")

include(compiler)

###############################################################################
# Dependencies
###############################################################################

find_package(Mstch REQUIRED)
find_package(Flatbuffers REQUIRED)

###############################################################################
# Compile protomodel
###############################################################################
add_executable(protomodel
  src/main.cpp
  src/explorer.cpp
  src/utils.cpp)
set_default_compiler_options(protomodel)
target_link_libraries(protomodel ${MSTCH_LIBRARIES} ${FLATBUFFERS_LIBRARIES})
target_include_directories(protomodel PUBLIC
  ${CMAKE_SOURCE_DIR}/include)
target_include_directories(protomodel
  SYSTEM PRIVATE
  ${MSTCH_INCLUDE_DIRS}
  ${FLATBUFFERS_INCLUDE_DIRS}
  third_parties/include)
target_compile_definitions(protomodel PRIVATE
  VERSION=\"${PROJECT_VERSION}\")


###############################################################################
# Compile protomerge
###############################################################################
add_executable(protomerge tools/protomerge.cpp)
set_default_compiler_options(protomerge)
target_include_directories(protomerge
  SYSTEM PRIVATE
  third_parties/include)
target_include_directories(protomerge PUBLIC
  ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(protomerge PRIVATE
  VERSION=\"${PROJECT_VERSION}\")

###############################################################################
# Installation
###############################################################################
install(TARGETS protomodel RUNTIME DESTINATION bin)
install(TARGETS protomerge RUNTIME DESTINATION bin)
# FIXME Don't install mstch
