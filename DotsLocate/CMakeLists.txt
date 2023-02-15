add_library(DotsLocate dots_locate.cxx)

find_package( OpenCV REQUIRED )

target_include_directories(DotsLocate
  PUBLIC ${OpenCV_INCLUDE_DIR}
)

target_link_libraries(DotsLocate
  PRIVATE ${OpenCV_LIBS} locating_dots_compiler_flags
)

target_include_directories(DotsLocate
          INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
          )

# install libs
set(installable_libs DotsLocate locating_dots_compiler_flags)
install(TARGETS ${installable_libs} DESTINATION lib)
# install include headers
install(FILES dots_locate.h DESTINATION include)
