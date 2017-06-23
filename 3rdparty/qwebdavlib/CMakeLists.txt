remove_definitions( -DQT_NO_CAST_FROM_ASCII )
remove_definitions( -DQT_NO_CAST_TO_ASCII )
remove_definitions( -DQT_NO_URL_CAST_FROM_STRING )
set(CMAKE_AUTOMOC TRUE)

add_library(qwebdavlib
    qwebdav.cpp
)

target_link_libraries(qwebdavlib
PUBLIC
    Qt5::Xml
    Qt5::Network
)
install(TARGETS
    qwebdavlib
    ${KDE_INSTALL_TARGETS_DEFAULT_ARGS}
)
