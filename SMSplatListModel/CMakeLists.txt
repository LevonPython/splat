set(CMAKE_AUTOMOC ON)

add_library(SMSplatListModel
    SMSplatListModel.h
    SMSplatListModel.cpp
    InputListModel.h
    InputListModel.cpp
)

target_link_libraries(SMSplatListModel PRIVATE
    Qt6::Core
    Qt6::Gui
    splat_utils
)

target_include_directories(SMSplatListModel
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../utils
    ${CMAKE_CURRENT_SOURCE_DIR}/../external/smsplat/utils
)

install(TARGETS SMSplatListModel DESTINATION lib)
