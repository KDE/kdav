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
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KF5/KDAV
    COMPONENT Devel
    )

install(FILES
    ${KDAV_HEADERS}
    ${CMAKE_CURRENT_BINARY_DIR}/kdav_export.h

    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KF5/kdav
    COMPONENT Devel
    )


ecm_qt_declare_logging_category(libkdav_SRCS
    HEADER libkdav_debug.h
    IDENTIFIER KDAV_LOG
    CATEGORY_NAME kf.dav
    OLD_CATEGORY_NAMES org.kde.pim.kdav
    DESCRIPTION "KDAV"
    EXPORT KDAV
)


add_library(KF5DAV
   ${libkdav_SRCS}
   )

generate_export_header(KF5DAV BASE_NAME kdav)

add_library(KF5::DAV ALIAS KF5DAV)

target_link_libraries(KF5DAV
PUBLIC
    KF5::CoreAddons
PRIVATE
    Qt5::Xml
    Qt5::Gui
    Qt5::XmlPatterns
    KF5::KIOCore
    KF5::I18n
    )

set_target_properties(KF5DAV PROPERTIES
    VERSION ${KDAV_VERSION}
    SOVERSION ${KDAV_SOVERSION}
    EXPORT_NAME DAV
    )

install(TARGETS
    KF5DAV
    EXPORT KF5DAVTargets
    ${KDE_INSTALL_TARGETS_DEFAULT_ARGS}
    ${LIBRARY_NAMELINK}
    )

ecm_generate_pri_file(BASE_NAME kdav
    LIB_NAME KF5DAV
    FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR}/KDAV
    )

install(FILES
    ${PRI_FILENAME}
    DESTINATION ${ECM_MKSPECS_INSTALL_DIR}
    )

target_include_directories(KF5DAV INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR}/KF5/KDAV/;${KDE_INSTALL_INCLUDEDIR}/KF5/kdav;${KDE_INSTALL_INCLUDEDIR}/KF5>")
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
