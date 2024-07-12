qt_standard_project_setup()

link_directories(${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})

qt_add_executable(app
    main.cpp
)

qt_add_qml_module(app
    URI app
    VERSION 1.0
    QML_FILES main.qml
    QML_FILES error_dialog.qml
    SOURCES filelistmodel.h filelistmodel.cpp
    SOURCES proxymodel.h proxymodel.cpp
    SOURCES worker.h worker.cpp
    SOURCES test.h
    SOURCES test.cpp
)

target_link_libraries(app PRIVATE
    core Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick
)

target_include_directories(app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../core
)
