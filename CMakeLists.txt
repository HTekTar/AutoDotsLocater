cmake_minimum_required(VERSION 3.15)

project(LocatingDots VERSION 1.0)

add_library(locating_dots_compiler_flags INTERFACE)
target_compile_features(locating_dots_compiler_flags INTERFACE cxx_std_11)

set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>")
set(msvc_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")
target_compile_options(locating_dots_compiler_flags INTERFACE
  "$<${gcc_like_cxx}:$<BUILD_INTERFACE:-Wall;-Wextra;-Wshadow;-Wformat=2;-Wunused>>"
  "$<${msvc_cxx}:$<BUILD_INTERFACE:-W3>>"
)

configure_file(LocatingDotsConfig.h.in LocatingDotsConfig.h)

add_subdirectory(DotsLocate)
list(APPEND EXTRA_LIBS DotsLocate)

add_executable(LocatingDots locate_dots.cxx)
target_link_libraries(LocatingDots PUBLIC ${EXTRA_LIBS} locating_dots_compiler_flags)

target_include_directories(LocatingDots PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           ${OpenCV_INCLUDE_DIR}
                           )

# add the install targets
install(TARGETS LocatingDots DESTINATION bin)
install(FILES "${PROJECT_BINARY_DIR}/LocatingDotsConfig.h"
  DESTINATION include
  )

# enable testing
include(CTest)

# does the application run
add_test(NAME Runs COMMAND LocatingDots --test 200)

# does the usage message work?
# add_test(NAME Usage COMMAND LocatingDots)
# set_tests_properties(Usage
#   PROPERTIES PASS_REGULAR_EXPRESSION "Usage:.*"
#   )


