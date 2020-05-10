cmake_minimum_required(VERSION 3.5)
set(PIM_VERSION "5.14.41")

project(libkdav VERSION ${PIM_VERSION})

set(LIBKDAV_VERSION ${PIM_VERSION})

set(KF5_MIN_VERSION "5.70.0")

find_package(ECM ${KF5_MIN_VERSION} CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)

include(CMakeFindDependencyMacro)
include(ECMSetupVersion)
include(GenerateExportHeader)
include(ECMGenerateHeaders)
include(ECMGeneratePriFile)
include(FeatureSummary)
include(ECMAddTests)
include(ECMInstallIcons)
include(ECMQtDeclareLoggingCategory)


set(QT_REQUIRED_VERSION "5.12.0")

find_package(Qt5 ${QT_REQUIRED_VERSION} CONFIG REQUIRED Core Gui XmlPatterns Test)

find_package(KF5 ${KF5_MIN_VERSION} REQUIRED CoreAddons KIO I18n)

# setup lib

if (EXISTS "${CMAKE_SOURCE_DIR}/.git")
   add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x050e00)
   add_definitions(-DKF_DISABLE_DEPRECATED_BEFORE_AND_AT=0x054600)
endif()

add_definitions(-DQT_NO_FOREACH)
add_definitions(-DQT_NO_KEYWORDS)



ecm_setup_version(${LIBKDAV_VERSION} VARIABLE_PREFIX KDAV
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/kdav_version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KF5DAVConfigVersion.cmake"
    SOVERSION 5
    )

set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KF5DAV")

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/KF5DAVConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/KF5DAVConfig.cmake"
    INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
    )

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/KF5DAVConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/KF5DAVConfigVersion.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    COMPONENT Devel
    )

install(EXPORT KF5DAVTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KF5DAVTargets.cmake NAMESPACE KF5::)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/kdav_version.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KF5 COMPONENT Devel
    )


add_subdirectory(src)
if(BUILD_TESTING)
    add_subdirectory(autotests)
    add_subdirectory(test)
endif()

feature_summary(WHAT ALL
                INCLUDE_QUIET_PACKAGES
                FATAL_ON_MISSING_REQUIRED_PACKAGES
)
