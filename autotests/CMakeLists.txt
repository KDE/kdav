set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
add_definitions(-DAUTOTEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data")

ecm_add_test(davcollectiontest.cpp davcollectiontest.h
    TEST_NAME davcollection
    NAME_PREFIX "kdav-"
    LINK_LIBRARIES KF5::DAV Qt${QT_MAJOR_VERSION}::Test Qt${QT_MAJOR_VERSION}::Core Qt${QT_MAJOR_VERSION}::Gui
)

ecm_add_test(davitemtest.cpp davitemtest.h
    TEST_NAME davitem
    NAME_PREFIX "kdav-"
    LINK_LIBRARIES KF5::DAV Qt${QT_MAJOR_VERSION}::Test Qt${QT_MAJOR_VERSION}::Core
)

ecm_add_test(davurltest.cpp davurltest.h
    TEST_NAME davurl
    NAME_PREFIX "kdav-"
    LINK_LIBRARIES KF5::DAV Qt${QT_MAJOR_VERSION}::Test Qt${QT_MAJOR_VERSION}::Core
)

ecm_add_test(davcollectionsmultifetchjobtest.cpp fakeserver.cpp davcollectionsmultifetchjobtest.h fakeserver.h
    TEST_NAME davcollectionsmultifetchjobtest
    NAME_PREFIX "kdav-"
    LINK_LIBRARIES KF5::DAV Qt${QT_MAJOR_VERSION}::Test Qt${QT_MAJOR_VERSION}::Core Qt${QT_MAJOR_VERSION}::Network
)

ecm_add_test(davitemfetchjobtest.cpp fakeserver.cpp davitemfetchjobtest.h fakeserver.h
    TEST_NAME davitemfetchjob
    NAME_PREFIX "kdav-"
    LINK_LIBRARIES KF5::DAV Qt${QT_MAJOR_VERSION}::Test Qt${QT_MAJOR_VERSION}::Core Qt${QT_MAJOR_VERSION}::Network
)

ecm_add_test(davitemslistjobtest.cpp fakeserver.cpp davitemslistjobtest.h fakeserver.h
    TEST_NAME davitemslistjob
    NAME_PREFIX "kdav-"
    LINK_LIBRARIES KF5::DAV Qt${QT_MAJOR_VERSION}::Test Qt${QT_MAJOR_VERSION}::Core Qt${QT_MAJOR_VERSION}::Network
)
