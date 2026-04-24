set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
add_definitions(-DAUTOTEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data")

set(ECM_TEST_NAME_PREFIX "kdav-")

ecm_add_test(davcollectiontest.cpp davcollectiontest.h
    TEST_NAME davcollection
    LINK_LIBRARIES KF6::DAV Qt6::Test Qt6::Core Qt6::Gui
)

ecm_add_test(davitemtest.cpp davitemtest.h
    TEST_NAME davitem
    LINK_LIBRARIES KF6::DAV Qt6::Test Qt6::Core
)

ecm_add_test(davurltest.cpp davurltest.h
    TEST_NAME davurl
    LINK_LIBRARIES KF6::DAV Qt6::Test Qt6::Core
)

ecm_add_test(davcollectionsmultifetchjobtest.cpp fakeserver.cpp davcollectionsmultifetchjobtest.h fakeserver.h
    TEST_NAME davcollectionsmultifetchjobtest
    LINK_LIBRARIES KF6::DAV Qt6::Test Qt6::Core Qt6::Network
)

ecm_add_test(davitemfetchjobtest.cpp fakeserver.cpp davitemfetchjobtest.h fakeserver.h
    TEST_NAME davitemfetchjob
    LINK_LIBRARIES KF6::DAV Qt6::Test Qt6::Core Qt6::Network
)

ecm_add_test(davcollectionsfetchjobtest.cpp fakeserver.cpp davcollectionsfetchjobtest.h fakeserver.h
    TEST_NAME davcollectionsfetchjobtest
    LINK_LIBRARIES KF6::DAV Qt6::Test Qt6::Core Qt6::Network Qt6::Gui
)

ecm_add_test(davitemslistjobtest.cpp fakeserver.cpp davitemslistjobtest.h fakeserver.h
    TEST_NAME davitemslistjob
    LINK_LIBRARIES KF6::DAV Qt6::Test Qt6::Core Qt6::Network
)

ecm_add_test(davprincipalsearchjobtest.cpp fakeserver.cpp davprincipalsearchjobtest.h fakeserver.h
    TEST_NAME davprincipalsearchjobtest
    LINK_LIBRARIES KF6::DAV Qt6::Test Qt6::Core Qt6::Network
)

ecm_add_test(davitemmodifyjobtest.cpp fakeserver.cpp davitemmodifyjobtest.h fakeserver.h
    TEST_NAME davitemmodifyjob
    LINK_LIBRARIES KF6::DAV Qt6::Test Qt6::Core Qt6::Network
)

ecm_add_test(davitemdeletejobtest.cpp fakeserver.cpp davitemdeletejobtest.h fakeserver.h
    TEST_NAME davitemdeletejobtest
    LINK_LIBRARIES KF6::DAV Qt6::Test Qt6::Core Qt6::Network
)

ecm_add_test(davcollectiondeletejobtest.cpp fakeserver.cpp davcollectiondeletejobtest.h fakeserver.h
    TEST_NAME davcollectiondeletejobtest
    LINK_LIBRARIES KF6::DAV Qt6::Test Qt6::Core Qt6::Network
)
