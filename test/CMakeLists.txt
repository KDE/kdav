kde_enable_exceptions()
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

add_executable(testserver testserver.cpp)

target_link_libraries(testserver
    Qt6::Core
    KF6::DAV
    Qt6::Xml
    )

