include_directories(
   common/
   protocols/
)

########### next target ###############

set(libkdav2_SRCS
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
 common/davjob.cpp

 protocols/groupdavprotocol.cpp

 #KContacts
 protocols/carddavprotocol.cpp

 #KCalCore
 protocols/caldavprotocol.cpp
)


ecm_generate_headers(KDAV2_Camelcase_HEADERS
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
    DavPrincipalHomesetsFetchJob
    DavPrincipalSearchJob
    DavUrl
    Utils
    Enums
    EtagCache
    REQUIRED_HEADERS KDAV2_HEADERS
    PREFIX KDAV2
    RELATIVE common
    )

install(FILES
    ${KDAV2_Camelcase_HEADERS}
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KPim/KDAV2
    COMPONENT Devel
    )

install(FILES
    ${KDAV2_HEADERS}
    ${CMAKE_CURRENT_BINARY_DIR}/kpimkdav2_export.h
    ${CMAKE_CURRENT_BINARY_DIR}/libkdav2_debug.h

    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KPim/kdav2
    COMPONENT Devel
    )

ecm_generate_pri_file(BASE_NAME kdav2
    LIB_NAME KPimKDAV2
    FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR}/KDAV2
    )

install(FILES
    ${PRI_FILENAME}
    DESTINATION ${ECM_MKSPECS_INSTALL_DIR}
    )

ecm_qt_declare_logging_category(libkdav2_SRCS HEADER libkdav2_debug.h IDENTIFIER KDAV2_LOG CATEGORY_NAME org.kde.pim.kdav2)

add_library(KPimKDAV2
   ${libkdav2_SRCS}
   )

generate_export_header(KPimKDAV2 BASE_NAME kpimkdav2)

add_library(KPim::KDAV2 ALIAS KPimKDAV2)

target_link_libraries(KPimKDAV2
PUBLIC
    KF5::CoreAddons
PRIVATE
    Qt5::Xml
    Qt5::Gui
    Qt5::XmlPatterns
    kdav2_webdavlib
    )

set_target_properties(KPimKDAV2 PROPERTIES
    VERSION ${KDAV2_VERSION_STRING}
    SOVERSION ${KDAV2_SOVERSION}
    EXPORT_NAME KDAV2
    )

install(TARGETS
    KPimKDAV2
    EXPORT KPimKDAV2Targets
    ${KDE_INSTALL_TARGETS_DEFAULT_ARGS}
    ${LIBRARY_NAMELINK}
    )

target_include_directories(KPimKDAV2 INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR}/KPim/KDAV2/;${KDE_INSTALL_INCLUDEDIR}/KPim/kdav2;${KDE_INSTALL_INCLUDEDIR}/KPim>")

