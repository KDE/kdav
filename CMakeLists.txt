cmake_minimum_required(VERSION 2.8.12)

project(libkdav2)

set(LIBKDAV2_VERSION "0.1.0")

configure_file(libkdav2-version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/libkdav2-version.h @ONLY)

set(KF5_VERSION "5.29.0")
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

find_package(Qt5 ${QT_REQUIRED_VERSION} CONFIG REQUIRED Core Gui Xml XmlPatterns Test)
find_package(KF5 ${KF5_VERSION} REQUIRED CoreAddons)

# setup lib

remove_definitions( -DQT_NO_CAST_FROM_ASCII )
remove_definitions( -DQT_NO_CAST_TO_ASCII )
remove_definitions( -DQT_NO_URL_CAST_FROM_STRING )

ecm_setup_version(${LIBKDAV2_VERSION} VARIABLE_PREFIX KDAV2
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/kpimkdav2_version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KPimKDAV2ConfigVersion.cmake"
    SOVERSION 5
    )

set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KPimKDAV2")

ecm_configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/KPimKDAV2Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/KPimKDAV2Config.cmake"
    INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
    )

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/KPimKDAV2Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/KPimKDAV2ConfigVersion.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    COMPONENT Devel
    )

install(EXPORT KPimKDAV2Targets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KPimKDAV2Targets.cmake NAMESPACE KPim::)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/kpimkdav2_version.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KPim COMPONENT Devel
    )

install(FILES kdav2.categories DESTINATION ${KDE_INSTALL_CONFDIR})


add_subdirectory(3rdparty)
include_directories(3rdparty)
add_subdirectory(src)
add_subdirectory(autotests)
add_subdirectory(test)

feature_summary(WHAT ALL
                INCLUDE_QUIET_PACKAGES
                FATAL_ON_MISSING_REQUIRED_PACKAGES
)
