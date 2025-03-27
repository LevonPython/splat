cmake_minimum_required(VERSION 3.14)
project(qt-smsplat)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Test Gui Sql Quick)
find_package(Eigen3 REQUIRED)
find_package(OpenCV REQUIRED COMPONENTS core imgproc imgcodecs highgui)

set(CMAKE_AUTOMOC ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/smsplat ${CMAKE_BINARY_DIR}/smsplat)
add_subdirectory(utils ${CMAKE_BINARY_DIR}/qt_splat_utils)
add_subdirectory(SMSplatListModel)
add_subdirectory(SMSplatDatabase)
add_subdirectory(QtSMSplat)
add_subdirectory(test ${CMAKE_BINARY_DIR}/splat_test)

include_directories(${OpenCV_INCLUDE_DIRS})
