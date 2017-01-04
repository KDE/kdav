set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

ecm_add_test(davcollectiontest.cpp
    TEST_NAME davcollection
    NAME_PREFIX "kdav-"
    LINK_LIBRARIES KPim::KDAV Qt5::Test Qt5::Core Qt5::Gui
)

ecm_add_test(davitemtest.cpp
    TEST_NAME davitem
    NAME_PREFIX "kdav-"
    LINK_LIBRARIES KPim::KDAV Qt5::Test Qt5::Core
)

ecm_add_test(davurltest.cpp
    TEST_NAME davurl
    NAME_PREFIX "kdav-"
    LINK_LIBRARIES KPim::KDAV Qt5::Test Qt5::Core
)
