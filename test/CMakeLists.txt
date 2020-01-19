kde_enable_exceptions()
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

set(testserver_SRCS
  testserver.cpp
)

add_executable(testserver ${testserver_SRCS})

target_link_libraries(testserver
    Qt5::Core
    KF5::DAV
    Qt5::Xml
    )

