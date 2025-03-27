cmake_minimum_required(VERSION 3.10)
project(SMSplatProject VERSION 1.0.0)

find_package(OpenCV REQUIRED COMPONENTS core imgproc imgcodecs highgui)
find_package(Eigen3 REQUIRED)
# Add subdirectories
add_subdirectory(SMSplat)
add_subdirectory(SMSplatManager)

# Enable testing
option(BUILD_TESTING "Build the testing tree." ON)
include(CTest)

# Add utils directory to include paths
include_directories(${CMAKE_SOURCE_DIR}/utils)
