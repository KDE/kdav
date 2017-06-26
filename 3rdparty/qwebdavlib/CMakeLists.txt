remove_definitions( -DQT_NO_CAST_FROM_ASCII )
remove_definitions( -DQT_NO_CAST_TO_ASCII )
remove_definitions( -DQT_NO_URL_CAST_FROM_STRING )
set(CMAKE_AUTOMOC TRUE)

add_library(kdav_webdavlib
    qwebdav.cpp
)

target_link_libraries(kdav_webdavlib
PUBLIC
    Qt5::Xml
    Qt5::Network
)
install(TARGETS
    kdav_webdavlib
    ${KDE_INSTALL_TARGETS_DEFAULT_ARGS}
)
