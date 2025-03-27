# Create SMSplatManager library
add_library(sm_splat_manager_lib
    src/sm_splat_manager.cpp
    src/gdal_handler.cpp
)

find_package(spdlog REQUIRED)
find_package(GDAL REQUIRED)
# Find OpenCV package
find_package(OpenCV REQUIRED COMPONENTS core imgproc imgcodecs highgui)
find_package(Eigen3 REQUIRED)




# Add include directories
target_include_directories(sm_splat_manager_lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/utils
        ${GDAL_INCLUDE_DIRS}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../SMSplat/src
        ${CMAKE_CURRENT_SOURCE_DIR}/../SMSplat/include
        ${OpenCV_INCLUDE_DIRS}
        ${Eigen3_INCLUDE_DIRS}
)


# Link against SPLAT library
target_link_libraries(sm_splat_manager_lib
    PRIVATE
        splat_lib
        spdlog::spdlog
        ${GDAL_LIBRARIES}
        ${OpenCV_LIBS}
        ${Eigen3_LIBRARIES}
        sm_utils
)

# Add the executable with a different name
add_executable(sm_splat_manager_exe src/main.cpp)

# Add include directories for the executable
target_include_directories(sm_splat_manager_exe
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../SMSplat/src
        ${CMAKE_CURRENT_SOURCE_DIR}/../SMSplat/include
)

target_link_libraries(sm_splat_manager_exe 
    PRIVATE 
        sm_splat_manager_lib 
        splat_lib 
        spdlog::spdlog
        GDAL::GDAL
        sm_utils
        ${OpenCV_LIBS}
        ${Eigen3_LIBRARIES}
)

# Optional: Set the output name of the executable to sm_splat_manager
set_target_properties(sm_splat_manager_exe
    PROPERTIES
    OUTPUT_NAME sm_splat_manager
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

