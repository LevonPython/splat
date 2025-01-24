# ============================== ADD UTILS SUBDIRECTORY ==============================

# utils/CMakeLists.txt
cmake_minimum_required(VERSION 3.10)

# ============================== SET COMMON COMPILER FLAGS ==============================

# Set common compiler flags used across all utilities
set(COMMON_FLAGS
    -O2
    -s
    -fomit-frame-pointer
)


# ============================== ADD INCLUDE DIRECTORIES ==============================

# Add include directories for utils
include_directories(${CMAKE_SOURCE_DIR}/src)


# ============================== ADD EXECUTABLES ==============================

# citydecoder
add_executable(citydecoder citydecoder.c)
target_compile_options(citydecoder PRIVATE ${COMMON_FLAGS})

# usgs2sdf
add_executable(usgs2sdf usgs2sdf.c)
target_compile_options(usgs2sdf PRIVATE ${COMMON_FLAGS})

# srtm2sdf
add_executable(srtm2sdf srtm2sdf.c)
target_compile_options(srtm2sdf PRIVATE ${COMMON_FLAGS})
target_link_libraries(srtm2sdf PRIVATE bz2)

# fontdata
add_executable(fontdata fontdata.c)
target_compile_options(fontdata PRIVATE ${COMMON_FLAGS})
target_link_libraries(fontdata PRIVATE z)

# bearing
add_executable(bearing bearing.c)
target_compile_options(bearing PRIVATE ${COMMON_FLAGS})
target_link_libraries(bearing PRIVATE m)

# ============================== CREATE SYMBOLIC LINK FOR srtm2sdf-hd ==============================

# Create symbolic link for srtm2sdf-hd
add_custom_command(
    TARGET srtm2sdf POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E rm -f srtm2sdf-hd
    COMMAND ${CMAKE_COMMAND} -E create_symlink srtm2sdf srtm2sdf-hd
)


# ============================== ADD INSTALLATION RULES ==============================

# Installation rules
install(TARGETS
    citydecoder
    usgs2sdf
    srtm2sdf
    fontdata
    bearing
    RUNTIME DESTINATION bin
)


# ============================== ADD CLEAN TARGET ==============================

# Clean target

# Clean target
add_custom_target(clean_utils
    COMMAND ${CMAKE_COMMAND} -E rm -f
        citydecoder
        usgs2sdf
        fontdata
        srtm2sdf
        srtm2sdf-hd
        bearing
)