# use the target_link_libraries command to instruct CMake to add our custom
# library as a dependency of this module's library (which always has the same
# name as the module itself)
target_link_libraries(sal-stack-ublox-odin-w2-binary "${CMAKE_CURRENT_LIST_DIR}/sal-stack-ublox-odin-w2-binary.a")