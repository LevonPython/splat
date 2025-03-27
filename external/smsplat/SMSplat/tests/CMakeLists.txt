# ============================== ADD TESTING SUBDIRECTORY ==============================

# ============================== FIND GTest PACKAGE ==============================

# Find GTest package
find_package(GTest REQUIRED)

# ============================== CREATE TESTS DIRECTORY IN BUILD ==============================

# Create tests directory in build
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/tests)

# ============================== PRINT STATUS MESSAGE ==============================

# Print status message
message(STATUS "Building test executable...")

# ============================== ADD TEST EXECUTABLE ==============================

# Add test executable
add_executable(splat_test splat_test.cpp)


# Add splat.cpp to the test executable
target_sources(splat_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src/splat.cpp)

# ============================== INCLUDE DIRECTORIES ==============================

# Include directories
target_include_directories(splat_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src      # For splat.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../utils    # For fontdata.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../include  # For splat_config.h
)

# ============================== LINK REQUIRED LIBRARIES ==============================

# Link against GTest and your main library
target_link_libraries(splat_test PRIVATE
    GTest::GTest
    GTest::Main
    m
    bz2
    splat_lib
)

# ============================== SET OUTPUT DIRECTORY FOR TEST EXECUTABLE ==============================


# Explicitly set the output directory for the test executable
set_target_properties(splat_test 
    PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/tests"
)

# ============================== ADD TEST ==============================

# Add the test
add_test(
    NAME splat_test
    COMMAND splat_test
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tests
)



# ============================== PRINT LOCATION OF TEST EXECUTABLE ==============================

# Print the location of the test executable (for debugging)
message(STATUS "Test executable will be built at: ${CMAKE_BINARY_DIR}/tests/splat_test")
