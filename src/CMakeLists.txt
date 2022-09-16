add_definitions(-DTRANSLATION_DOMAIN=\"libkdav\")

set(KDAV_INSTALL_INCLUDEDIR "${KDE_INSTALL_INCLUDEDIR_KF}/KDAV")

add_library(KF5DAV)
add_library(KF5::DAV ALIAS KF5DAV)

set_target_properties(KF5DAV PROPERTIES
    VERSION     ${KDAV_VERSION}
    SOVERSION   ${KDAV_SOVERSION}
    EXPORT_NAME DAV
)

target_sources(KF5DAV PRIVATE
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

ecm_qt_declare_logging_category(KF5DAV
    HEADER libkdav_debug.h
    IDENTIFIER KDAV_LOG
    CATEGORY_NAME kf.dav
    OLD_CATEGORY_NAMES org.kde.pim.kdav
    DESCRIPTION "KDAV"
    EXPORT KDAV
)

generate_export_header(KF5DAV BASE_NAME kdav)

target_include_directories(KF5DAV PRIVATE
   common
   protocols
)

target_link_libraries(KF5DAV
PUBLIC
    KF5::CoreAddons
PRIVATE
    Qt${QT_MAJOR_VERSION}::Xml
    Qt${QT_MAJOR_VERSION}::Gui
    KF5::KIOCore
    KF5::I18n
    )


install(TARGETS
    KF5DAV
    EXPORT KF5DAVTargets
    ${KF_INSTALL_TARGETS_DEFAULT_ARGS}
    )

ecm_generate_pri_file(BASE_NAME KDAV
    LIB_NAME KF5DAV
    DEPS "KCoreAddons"
    FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KDAV_INSTALL_INCLUDEDIR}
    )

install(FILES
    ${PRI_FILENAME}
    DESTINATION ${ECM_MKSPECS_INSTALL_DIR}
    )

target_include_directories(KF5DAV INTERFACE "$<INSTALL_INTERFACE:${KDAV_INSTALL_INCLUDEDIR}>")

ecm_qt_install_logging_categories(EXPORT KDAV FILE kdav.categories DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR})

if (BUILD_QCH)
    ecm_add_qch(
        KF5DAV_QCH
        NAME KDAV
        BASE_NAME KF5DAV
        VERSION ${KF_VERSION}
        ORG_DOMAIN org.kde
        SOURCES # using only public headers, to cover only public API
            ${KDAV_HEADERS}
        MD_MAINPAGE "${CMAKE_SOURCE_DIR}/README.md"
        LINK_QCHS
            Qt5Gui_QCH
            KF5CoreAddons_QCH
        INCLUDE_DIRS
            ${CMAKE_CURRENT_BINARY_DIR}
        BLANK_MACROS
            KDAV_EXPORT
            KDAV_DEPRECATED_EXPORT
            KDAV_DEPRECATED
            "KDAV_DEPRECATED_VERSION(x, y, t)"
        TAGFILE_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        QCH_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        COMPONENT Devel
    )
endif()
