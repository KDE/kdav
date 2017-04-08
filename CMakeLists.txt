cmake_minimum_required(VERSION 2.8.12)

project(libkdav)

set(LIBKDAV_VERSION "5.5.0")

configure_file(libkdav-version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/libkdav-version.h @ONLY)

set(KF5_VERSION "5.32.0")
find_package(ECM ${KF5_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(ECMPackageConfigHelpers)
include(ECMSetupVersion)
include(GenerateExportHeader)
include(ECMGenerateHeaders)
include(ECMGeneratePriFile)
include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(ECMAddTests)
include(ECMInstallIcons)
include(ECMQtDeclareLoggingCategory)
include(ECMCoverageOption)

set(QT_REQUIRED_VERSION "5.6.0")

find_package(Qt5 ${QT_REQUIRED_VERSION} CONFIG REQUIRED Core Gui XmlPatterns Test)

find_package(KF5 ${KF5_VERSION} REQUIRED CoreAddons KIO)

# setup lib

add_definitions( -DQT_NO_CAST_FROM_ASCII )
add_definitions( -DQT_NO_CAST_TO_ASCII )
add_definitions( -DQT_NO_URL_CAST_FROM_STRING )

ecm_setup_version(${LIBKDAV_VERSION} VARIABLE_PREFIX KDAV
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/kpimkdav_version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KPimKDAVConfigVersion.cmake"
    SOVERSION 5
    )

set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KPimKDAV")

ecm_configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/KPimKDAVConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/KPimKDAVConfig.cmake"
    INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
    )

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/KPimKDAVConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/KPimKDAVConfigVersion.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    COMPONENT Devel
    )

install(EXPORT KPimKDAVTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KPimKDAVTargets.cmake NAMESPACE KPim::)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/kpimkdav_version.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KPim COMPONENT Devel
    )

install(FILES kdav.categories DESTINATION ${KDE_INSTALL_CONFDIR})

add_subdirectory(src)
add_subdirectory(autotests)
add_subdirectory(test)

feature_summary(WHAT ALL
                INCLUDE_QUIET_PACKAGES
                FATAL_ON_MISSING_REQUIRED_PACKAGES
)
