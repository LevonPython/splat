cmake_minimum_required(VERSION 3.10)

# Source files
set(UTILS_SOURCES
    sm_splat_info.cpp
    image_utils.cpp
)

# Header files
set(UTILS_HEADERS
    sm_splat_info.h
    image_utils.h
)

# Create library
add_library(sm_utils STATIC
    ${UTILS_SOURCES}
    ${UTILS_HEADERS}
)

# Set include directories
target_include_directories(sm_utils PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(sm_utils PUBLIC
    ${OpenCV_LIBS}
)

# C++ standard
set_target_properties(sm_utils PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
