# ============================== Basic Setup ==============================
cmake_minimum_required(VERSION 3.10)
project(SPLAT VERSION 1.5.0)

# ============================== BUILD_TESTING, HD_MODE and MAXPAGES flags for building options ==============================
# Enable testing
option(BUILD_TESTING "Build the testing tree." ON)
include(CTest)

# Add configuration options with default values
option(HD_MODE "Enable HD mode" OFF)  # OFF = 0, ON = 1
set(MAXPAGES "4" CACHE STRING "Set MAXPAGES value (1-64)")
# cmake -DHD_MODE=ON -DMAXPAGES=8 ..

# ============================== VALIDATE BUILD OPTIONS ==============================
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
endif()


# ============================== GENERATE splat_config.h ==============================

# Then the configure_file command
configure_file(
    "${CMAKE_SOURCE_DIR}/cmake/splat_config.h.in"
    "${CMAKE_SOURCE_DIR}/include/splat_config.h"
)


# ============================== DETECT CPU ARCHITECTURE AND SET MODEL FLAGS ==============================

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


# ============================== ADD SPLAT EXECUTABLE ==============================
# UNCOMMENT THIS TO BUILD SPLAT EXECUTABLE WITHOUT TESTS
## Main SPLAT executable
#add_executable(splat src/splat.cpp)

# Create SPLAT library instead of executable
# COMMENT THIS IN CASE BULDING EXECUTABLE FILE
add_library(splat_lib STATIC src/splat.cpp)

# Add include directories
target_include_directories(splat_lib PRIVATE
    ${CMAKE_SOURCE_DIR}/src      # For splat.h
    ${CMAKE_SOURCE_DIR}/utils    # For fontdata.h and other utility headers
    ${CMAKE_SOURCE_DIR}/include  # For generated splat_config.h
)


# ============================== SET COMPILER FLAGS ==============================

# Set compiler flags to match the original command
target_compile_options(splat_lib PRIVATE
    -g
    -O2
    -fomit-frame-pointer
    -ffast-math
    -pipe
    $<$<BOOL:${CPU_ARCH}>:-march=${CPU_ARCH}>
    ${MODEL_FLAGS}
)


# ============================== LINK REQUIRED LIBRARIES ==============================

# Link required libraries
target_link_libraries(splat_lib PRIVATE
    m
    bz2
)


# ============================== ADD UTILS SUBDIRECTORY ==============================

# Add utils subdirectory
add_subdirectory(utils)


# ============================== ADD CUSTOM TARGET TO BUILD EVERYTHING ==============================

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

# ============================== ADD TESTING SUBDIRECTORY ==============================

# Add testing if enabled
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()


# ============================== ADD INSTALLATION RULES ==============================

# Installation rules
install(TARGETS splat_lib
    RUNTIME DESTINATION bin
)


#rm -rf build
#mkdir build
#cd build
#cmake -DBUILD_TESTING=ON ..
#cmake --build . --target build_all
