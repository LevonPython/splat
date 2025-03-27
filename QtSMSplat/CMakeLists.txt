set(QML_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../smsplat.qrc)
qt_add_library(QtSMSplat STATIC
    QtSMSplat.h
    QtSMSplat.cpp
    SMSplatImageProvider.h
    SMSplatImageProvider.cpp
    ${QML_RESOURCES}
)

qt_add_resources(QtSMSplat ${QML_RESOURCES})

target_link_libraries(QtSMSplat PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
    Qt6::Sql
    SMSplatListModel
    SMSplatDB
    sm_splat_manager_lib
    splat_utils
    ${OpenCV_LIBS}
)

target_include_directories(QtSMSplat
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../utils
    ${CMAKE_CURRENT_SOURCE_DIR}/../SMSplatListModel
    ${CMAKE_CURRENT_SOURCE_DIR}/../SMSplatDatabase
    ${CMAKE_CURRENT_SOURCE_DIR}/../external/smsplat/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/../external/smsplat/SMSplatManager/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../external/smsplat/SMSplat/src
    ${OpenCV_INCLUDE_DIRS}
)

set_target_properties(QtSMSplat PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
)