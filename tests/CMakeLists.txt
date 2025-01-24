# Find GTest package
find_package(GTest REQUIRED)

# Create tests directory in build
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/tests)

# Print status message
message(STATUS "Building test executable...")

# Add test executable
add_executable(splat_test splat_test.cpp)

# Include directories
target_include_directories(splat_test PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/include
)

# Link against GTest and your main library
target_link_libraries(splat_test PRIVATE
    GTest::GTest
    GTest::Main
    m
    bz2
)

# Explicitly set the output directory for the test executable
set_target_properties(splat_test 
    PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/tests"
)

# Add the test
add_test(
    NAME splat_test
    COMMAND splat_test
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tests
)


# Print the location of the test executable (for debugging)
message(STATUS "Test executable will be built at: ${CMAKE_BINARY_DIR}/tests/splat_test")