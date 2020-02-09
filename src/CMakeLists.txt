add_definitions(-DTRANSLATION_DOMAIN=\"libkdav\")
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
 common/protocolinfo.cpp
 common/davurl.cpp
 common/utils.cpp
 common/etagcache.cpp

 protocols/groupdavprotocol.cpp

 #KContacts
 protocols/carddavprotocol.cpp

 #KCalCore
 protocols/caldavprotocol.cpp
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
    DavPrincipalHomesetsFetchJob
    DavPrincipalSearchJob
    DavUrl
    Enums
    EtagCache
    ProtocolInfo
    REQUIRED_HEADERS KDAV_HEADERS
    PREFIX KDAV
    RELATIVE common
    )

install(FILES
    ${KDAV_Camelcase_HEADERS}
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KPim/KDAV
    COMPONENT Devel
    )

install(FILES
    ${KDAV_HEADERS}
    ${CMAKE_CURRENT_BINARY_DIR}/kpimkdav_export.h

    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KPim/kdav
    COMPONENT Devel
    )

ecm_generate_pri_file(BASE_NAME kdav
    LIB_NAME KPimKDAV
    FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR}/KDAV
    )

install(FILES
    ${PRI_FILENAME}
    DESTINATION ${ECM_MKSPECS_INSTALL_DIR}
    )

ecm_qt_declare_logging_category(libkdav_SRCS HEADER libkdav_debug.h IDENTIFIER KDAV_LOG CATEGORY_NAME org.kde.pim.kdav)

add_library(KPimKDAV
   ${libkdav_SRCS}
   )

generate_export_header(KPimKDAV BASE_NAME kpimkdav)

add_library(KPim::KDAV ALIAS KPimKDAV)

target_link_libraries(KPimKDAV
PUBLIC
    KF5::CoreAddons
PRIVATE
    Qt5::Xml
    Qt5::Gui
    Qt5::XmlPatterns
    KF5::KIOCore
    KF5::I18n
    )

set_target_properties(KPimKDAV PROPERTIES
    VERSION ${KDAV_VERSION_STRING}
    SOVERSION ${KDAV_SOVERSION}
    EXPORT_NAME KDAV
    )

install(TARGETS
    KPimKDAV
    EXPORT KPimKDAVTargets
    ${KDE_INSTALL_TARGETS_DEFAULT_ARGS}
    ${LIBRARY_NAMELINK}
    )

target_include_directories(KPimKDAV INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR}/KPim/KDAV/;${KDE_INSTALL_INCLUDEDIR}/KPim/kdav;${KDE_INSTALL_INCLUDEDIR}/KPim>")

