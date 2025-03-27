add_library(splat_utils STATIC)

target_sources(splat_utils PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Type.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Type.h
)

target_include_directories(splat_utils PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../external/smsplat/utils
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(splat_utils
    Qt6::Core
    Qt6::Gui
    sm_utils
    ${OpenCV_LIBS}
)

