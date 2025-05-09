add_definitions(-DTRANSLATION_DOMAIN=\"libkdav6\")

set(KDAV_INSTALL_INCLUDEDIR "${KDE_INSTALL_INCLUDEDIR_KF}/KDAV")

add_library(KF6DAV)
add_library(KF6::DAV ALIAS KF6DAV)

set_target_properties(KF6DAV PROPERTIES
    VERSION     ${KDAV_VERSION}
    SOVERSION   ${KDAV_SOVERSION}
    EXPORT_NAME DAV
)

target_sources(KF6DAV PRIVATE
    common/davcollection.cpp
    common/davcollectiondeletejob.cpp
    common/davcollectiondeletejob.h
    common/davcollection.h
    common/davcollectionmodifyjob.cpp
    common/davcollectionmodifyjob.h
    common/davcollectionsfetchjob.cpp
    common/davcollectionsfetchjob.h
    common/davcollectionsmultifetchjob.cpp
    common/davcollectionsmultifetchjob.h
    common/daverror.cpp
    common/daverror.h
    common/davitem.cpp
    common/davitemcreatejob.cpp
    common/davitemcreatejob.h
    common/davitemdeletejob.cpp
    common/davitemdeletejob.h
    common/davitemfetchjob.cpp
    common/davitemfetchjob.h
    common/davitem.h
    common/davitemmodifyjob.cpp
    common/davitemmodifyjob.h
    common/davitemsfetchjob.cpp
    common/davitemsfetchjob.h
    common/davitemslistjob.cpp
    common/davitemslistjob.h
    common/davjobbase.cpp
    common/davjobbase.h
    common/davjobbase_p.h
    common/davmanager.cpp
    common/davmanager_p.h
    common/davmultigetprotocol.cpp
    common/davmultigetprotocol_p.h
    common/davprincipalhomesetsfetchjob.cpp
    common/davprincipalhomesetsfetchjob.h
    common/davprincipalsearchjob.cpp
    common/davprincipalsearchjob.h
    common/davprotocolbase.cpp
    common/davprotocolbase_p.h
    common/davurl.cpp
    common/davurl.h
    common/enums.h
    common/etagcache.cpp
    common/etagcache.h
    common/protocolinfo.cpp
    common/protocolinfo.h
    common/utils.cpp
    common/utils_p.h

    protocols/groupdavprotocol.cpp
    protocols/groupdavprotocol_p.h

    #KContacts
    protocols/carddavprotocol.cpp
    protocols/carddavprotocol_p.h

    #KCalCore
    protocols/caldavprotocol.cpp
    protocols/caldavprotocol_p.h
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
    DESTINATION ${KDAV_INSTALL_INCLUDEDIR}/KDAV # for C++ namespace KDAV
    COMPONENT Devel
    )

install(FILES
    ${KDAV_HEADERS}
    ${CMAKE_CURRENT_BINARY_DIR}/kdav_export.h

    DESTINATION ${KDAV_INSTALL_INCLUDEDIR}/kdav # for C++ namespace KDAV
    COMPONENT Devel
    )

ecm_qt_declare_logging_category(KF6DAV
    HEADER libkdav_debug.h
    IDENTIFIER KDAV_LOG
    CATEGORY_NAME kf.dav
    OLD_CATEGORY_NAMES org.kde.pim.kdav
    DESCRIPTION "KDAV"
    EXPORT KDAV
)

ecm_generate_export_header(KF6DAV
    BASE_NAME kdav
    GROUP_BASE_NAME KF
    VERSION ${KF_VERSION}
    USE_VERSION_HEADER
    DEPRECATED_BASE_VERSION 0
)

target_include_directories(KF6DAV PRIVATE
    common
    protocols
)

target_link_libraries(KF6DAV
PUBLIC
    KF6::CoreAddons
PRIVATE
    Qt6::Xml
    Qt6::Gui
    KF6::KIOCore
    KF6::I18n
    )


install(TARGETS
    KF6DAV
    EXPORT KF6DAVTargets
    ${KF_INSTALL_TARGETS_DEFAULT_ARGS}
    )

target_include_directories(KF6DAV INTERFACE "$<INSTALL_INTERFACE:${KDAV_INSTALL_INCLUDEDIR}>")

ecm_qt_install_logging_categories(EXPORT KDAV FILE kdav.categories DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR})

ecm_generate_qdoc(KF6DAV kdav.qdocconf)
