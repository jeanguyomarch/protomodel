function (set_default_compiler_options Target)
  if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_compile_options(${Target}
      PRIVATE
      -Wall
      -Wextra
      -Wshadow
      -Winit-self
      -Wfloat-equal
      -Wtrigraphs
      -Wconversion
      -Wcast-align
      -Wlogical-op
      -Wunsafe-loop-optimizations
      -Wdouble-promotion
      -Wformat=2
      -fstrict-enums
    )
  endif ()
  set_property(TARGET ${Target} PROPERTY CXX_STANDARD 17)
endfunction ()

