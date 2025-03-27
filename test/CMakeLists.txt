cmake_minimum_required(VERSION 3.14)
project(test_smsplatdb)
set(QML_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../smsplat.qrc)

add_executable(test_smsplatdb 
    test_smsplatdb.cpp
)

add_executable(test_smsplat_list_model 
    test_smsplat_list_model.cpp
)

add_executable(test_smsplat_ui 
    test_QtSMSplat_UI.cpp
)

add_executable(splat_app
    splat_app.cpp
    ${QML_RESOURCES}
)

add_executable(splat_app_ui
    splat_app_ui.cpp
    ${QML_RESOURCES}
)

qt_add_resources(test_smsplat_ui ${QML_RESOURCES})
qt_add_resources(splat_app ${QML_RESOURCES})
qt_add_resources(splat_app_ui ${QML_RESOURCES})


set_target_properties(test_smsplat_ui PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
)

set_target_properties(splat_app PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
)

set_target_properties(splat_app_ui PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
)

set_target_properties(test_smsplat_list_model PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
)

set_target_properties(splat_app PROPERTIES
    QT_QML_FILES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../qml
)

set_target_properties(splat_app_ui PROPERTIES
    QT_QML_FILES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../qml
)

target_link_libraries(test_smsplatdb
    PRIVATE
    Qt6::Core
    Qt6::Test
    Qt6::Gui
    Qt6::Sql
    SMSplatDB
)

target_link_libraries(test_smsplat_list_model
    PRIVATE
    Qt6::Core
    Qt6::Test
    Qt6::Gui
    SMSplatListModel
)

target_link_libraries(test_smsplat_ui
    PRIVATE
    Qt6::Core
    Qt6::Test
    Qt6::Gui
    Qt6::Quick
    Qt6::Sql
    QtSMSplat
)

target_link_libraries(splat_app
    PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
    Qt6::Sql
    QtSMSplat
)

target_link_libraries(splat_app_ui
    PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
    Qt6::Sql
    QtSMSplat
)

target_include_directories(test_smsplat_ui
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../QtSMSplat
)

target_include_directories(splat_app
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../QtSMSplat
)

target_include_directories(splat_app_ui
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../QtSMSplat
)

target_include_directories(test_smsplat_list_model
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../SMSplatListModel
    ${OpenCV_INCLUDE_DIRS}
)

add_test(NAME test_smsplatdb COMMAND test_smsplatdb)
add_test(NAME test_smsplat_list_model COMMAND test_smsplat_list_model)
add_test(NAME test_smsplat_ui COMMAND test_smsplat_ui)