kde_enable_exceptions()
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

add_executable(testserver testserver.cpp)

target_link_libraries(testserver
    Qt${QT_MAJOR_VERSION}::Core
    KF5::DAV
    Qt${QT_MAJOR_VERSION}::Xml
    )

