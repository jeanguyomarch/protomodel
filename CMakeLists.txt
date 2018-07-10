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
add_subdirectory(third_parties/mstch)
add_subdirectory(third_parties/flatbuffers)
list(APPEND PROTOMODEL_LIBS mstch flatbuffers)
list(APPEND PROTOMODEL_INCLUDE_DIRS
     ${mstch_INCLUDE_DIR}
     third_parties/flatbuffers/include)

###############################################################################
# Compile protomodel
###############################################################################
list(APPEND PROTOMODEL_SOURCES
     src/main.cpp
     src/explorer.cpp
     src/utils.cpp)
list(APPEND PROTOMODEL_INCLUDE_DIRS
     third_parties/include)
add_executable(protomodel ${PROTOMODEL_SOURCES})
set_default_compiler_options(protomodel)
target_link_libraries(protomodel ${PROTOMODEL_LIBS})
target_include_directories(protomodel PUBLIC
  ${CMAKE_SOURCE_DIR}/include)
target_include_directories(protomodel
  SYSTEM PRIVATE ${PROTOMODEL_INCLUDE_DIRS})
target_compile_definitions(protomodel PRIVATE
  VERSION=\"${PROJECT_VERSION}\")

###############################################################################
# Installation
###############################################################################
install(TARGETS protomodel RUNTIME DESTINATION bin)
# FIXME Don't install mstch
