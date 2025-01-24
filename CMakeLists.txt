cmake_minimum_required(VERSION 3.10)
project(SPLAT VERSION 1.4.3)

# Enable testing
option(BUILD_TESTING "Build the testing tree." ON)
include(CTest)

# Add configuration options with default values
option(HD_MODE "Enable HD mode" OFF)  # OFF = 0, ON = 1
set(MAXPAGES "4" CACHE STRING "Set MAXPAGES value (1-64)")

# Convert HD_MODE boolean to 0/1
if(HD_MODE)
    set(HD_MODE_VALUE 1)
else()
    set(HD_MODE_VALUE 0)
endif()

# Validate MAXPAGES
if(NOT MAXPAGES MATCHES "^[1-9]|[1-5][0-9]|6[0-4]$")
    message(WARNING "MAXPAGES must be between 1 and 64, setting to default value 4")
    set(MAXPAGES "4")
endif()# Add configuration options
option(HD_MODE "Enable HD mode" OFF)
set(MAXPAGES "4" CACHE STRING "Set MAXPAGES value")

## Create binary directory for generated files
#set(GENERATED_DIR "${CMAKE_BINARY_DIR}/generated")
#file(MAKE_DIRECTORY ${GENERATED_DIR})

# Then the configure_file command
configure_file(
    "${CMAKE_SOURCE_DIR}/cmake/splat_config.h.in"
    "${CMAKE_SOURCE_DIR}/include/splat_config.h"
)

# Detect CPU architecture and set model flags
if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
    set(CPU_ARCH "x86-64")
    if(APPLE)
        set(MODEL_FLAGS "")
    else()
        set(MODEL_FLAGS "-mcmodel=medium")
    endif()
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "sun4u")
    set(CPU_ARCH "")
    set(MODEL_FLAGS "")
endif()

# Main SPLAT executable
add_executable(splat src/splat.cpp)

# Add include directories
target_include_directories(splat PRIVATE
    ${CMAKE_SOURCE_DIR}/src      # For splat.h
    ${CMAKE_SOURCE_DIR}/utils    # For fontdata.h and other utility headers
    ${CMAKE_SOURCE_DIR}/include  # For generated splat_config.h
)

# Set compiler flags to match the original command
target_compile_options(splat PRIVATE
    -g
    -O2
    -fomit-frame-pointer
    -ffast-math
    -pipe
    $<$<BOOL:${CPU_ARCH}>:-march=${CPU_ARCH}>
    ${MODEL_FLAGS}
)

# Link required libraries
target_link_libraries(splat PRIVATE
    m
    bz2
)

# Add utils subdirectory
add_subdirectory(utils)


# Add custom target to build everything
add_custom_target(build_all ALL
    DEPENDS 
        splat
        citydecoder
        usgs2sdf
        srtm2sdf
        fontdata
        bearing
        splat_test
)

# Add testing if enabled
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

# Installation rules
install(TARGETS splat
    RUNTIME DESTINATION bin
)