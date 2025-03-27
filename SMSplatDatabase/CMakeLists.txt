cmake_minimum_required(VERSION 3.14)
project(SMSplatDatabase)

# Set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(Qt6 REQUIRED COMPONENTS Core Sql Gui)

set(CMAKE_AUTOMOC ON)

add_library(SMSplatDB
    SMSplatDB.cpp
    SMSplatDB.h
)

target_link_libraries(SMSplatDB
    PRIVATE
    Qt6::Core
    Qt6::Sql
    Qt6::Gui
    splat_utils
)

target_include_directories(SMSplatDB PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../utils
    ${CMAKE_CURRENT_SOURCE_DIR}/../external/smsplat/utils
    ${OpenCV_INCLUDE_DIRS}
)
