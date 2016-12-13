kde_enable_exceptions()
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

set(testserver_SRCS
  testserver.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src)
add_executable(testserver ${testserver_SRCS})

target_link_libraries(testserver
    Qt5::Core
    Qt5::Gui
    KF5::CoreAddons
    libkdav
    )

