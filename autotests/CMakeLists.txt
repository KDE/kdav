set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
add_definitions(-DAUTOTEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data")

ecm_add_test(davcollectiontest.cpp davcollectiontest.h
    TEST_NAME davcollection
    NAME_PREFIX "kdav-"
    LINK_LIBRARIES KF6::DAV Qt6::Test Qt6::Core Qt6::Gui
)

ecm_add_test(davitemtest.cpp davitemtest.h
    TEST_NAME davitem
    NAME_PREFIX "kdav-"
    LINK_LIBRARIES KF6::DAV Qt6::Test Qt6::Core
)

ecm_add_test(davurltest.cpp davurltest.h
    TEST_NAME davurl
    NAME_PREFIX "kdav-"
    LINK_LIBRARIES KF6::DAV Qt6::Test Qt6::Core
)

ecm_add_test(davcollectionsmultifetchjobtest.cpp fakeserver.cpp davcollectionsmultifetchjobtest.h fakeserver.h
    TEST_NAME davcollectionsmultifetchjobtest
    NAME_PREFIX "kdav-"
    LINK_LIBRARIES KF6::DAV Qt6::Test Qt6::Core Qt6::Network
)

ecm_add_test(davitemfetchjobtest.cpp fakeserver.cpp davitemfetchjobtest.h fakeserver.h
    TEST_NAME davitemfetchjob
    NAME_PREFIX "kdav-"
    LINK_LIBRARIES KF6::DAV Qt6::Test Qt6::Core Qt6::Network
)

ecm_add_test(davitemslistjobtest.cpp fakeserver.cpp davitemslistjobtest.h fakeserver.h
    TEST_NAME davitemslistjob
    NAME_PREFIX "kdav-"
    LINK_LIBRARIES KF6::DAV Qt6::Test Qt6::Core Qt6::Network
)
