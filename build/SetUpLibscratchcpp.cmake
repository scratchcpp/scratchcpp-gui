set(LIBSCRATCHCPP_BUILD_UNIT_TESTS OFF)
add_subdirectory(${PROJECT_SOURCE_DIR}/libscratchcpp libscratchcpp)
target_include_directories(libscratchcpp-gui PUBLIC ${PROJECT_SOURCE_DIR}/libscratchcpp/include)
