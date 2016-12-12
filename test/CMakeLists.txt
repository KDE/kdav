kde_enable_exceptions()
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

set(testserver_SRCS
  testserver.cpp
  ../src/common/davmanager.cpp
  ../src/common/davcollection.cpp
  ../src/common/davcollectionsfetchjob.cpp
  ../src/common/davitem.cpp
  ../src/common/davitemfetchjob.cpp
  ../src/common/davitemsfetchjob.cpp
  ../src/common/davitemslistjob.cpp
  ../src/common/davjobbase.cpp
  ../src/common/davmultigetprotocol.cpp
  ../src/common/davprincipalhomesetsfetchjob.cpp
  ../src/common/davprotocolbase.cpp
  ../src/common/davutils.cpp
  ../src/common/etagcache.cpp
  ../src/protocols/groupdavprotocol.cpp
  ../src/protocols/carddavprotocol.cpp
  ../src/protocols/caldavprotocol.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src)
add_executable(testserver ${testserver_SRCS})

target_link_libraries(testserver
    Qt5::Core
    Qt5::Gui
    Qt5::Xml
    Qt5::XmlPatterns
    KF5::KIOCore
    KF5::I18n
    )

