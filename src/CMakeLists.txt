include_directories(
   common/
   protocols/
)

########### next target ###############

set(libkdav_SRCS
 common/davjobbase.cpp
 common/davcollection.cpp
 common/davcollectiondeletejob.cpp
 common/davcollectionsfetchjob.cpp
 common/davcollectionmodifyjob.cpp
 common/davcollectionsmultifetchjob.cpp
 common/davprotocolbase.cpp
 common/daverror.cpp
 common/davitem.cpp
 common/davitemcreatejob.cpp
 common/davitemdeletejob.cpp
 common/davitemfetchjob.cpp
 common/davitemmodifyjob.cpp
 common/davitemsfetchjob.cpp
 common/davitemslistjob.cpp
 common/davmanager.cpp
 common/davmultigetprotocol.cpp
 common/davprincipalhomesetsfetchjob.cpp
 common/davprincipalsearchjob.cpp
 common/davurl.cpp
 common/utils.cpp
 common/etagcache.cpp

 protocols/groupdavprotocol.cpp

 #KContacts
 protocols/carddavprotocol.cpp

 #KCalCore
 protocols/caldavprotocol.cpp

 #With Akonadi reference
 #common/etagcache-akonadi.cpp

 #common/davutils-akonadi.cpp
 #resource/davprotocolattribute.cpp
)


ecm_generate_headers(KDAV_Camelcase_HEADERS
    HEADER_NAMES
    DavJobBase
    DavCollection
    DavCollectionDeleteJob
    DavCollectionsFetchJob
    DavCollectionModifyJob
    DavCollectionsMultiFetchJob
    DavError
    DavItem
    DavItemCreateJob
    DavItemDeleteJob
    DavItemFetchJob
    DavItemModifyJob
    DavItemsFetchJob
    DavItemsListJob
    DavManager
    DavProtocolBase
    DavUrl
    Utils
    Enums
    EtagCache
    REQUIRED_HEADERS KDAV_HEADERS
    PREFIX KDAV
    RELATIVE common
    )

install(FILES
    ${KDAV_Camelcase_HEADERS}
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KDAV
    COMPONENT Devel
    )

install(FILES
    ${KDAV_HEADERS}
    ${CMAKE_CURRENT_BINARY_DIR}/libkdav_export.h
    ${CMAKE_CURRENT_BINARY_DIR}/libkdav_debug.h

    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/kdav
    COMPONENT Devel
    )

ecm_generate_pri_file(BASE_NAME kdav
    LIB_NAME kdav
    FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR_KF5}/KDAV
    )

install(FILES
    ${PRI_FILENAME}
    DESTINATION ${ECM_MKSPECS_INSTALL_DIR}
    )

ecm_qt_declare_logging_category(libkdav_SRCS HEADER libkdav_debug.h IDENTIFIER KDAV_LOG CATEGORY_NAME org.kde.pim.kdav)

add_library(kdav
   ${libkdav_SRCS}
   )

generate_export_header(kdav BASE_NAME libkdav)

target_link_libraries(kdav
PUBLIC
    KF5::CoreAddons
PRIVATE
    Qt5::Xml
    Qt5::Gui
    Qt5::XmlPatterns
    KF5::KIOCore
    KF5::I18n
    #KF5::Akonadi
    )


install(TARGETS
    kdav
    EXPORT KDAVTargets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS} ${LIBRARY_NAMELINK}
    )
