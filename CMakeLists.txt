cmake_minimum_required(VERSION 3.5)
set(PIM_VERSION "5.13.40")

project(libkdav VERSION ${PIM_VERSION})

set(LIBKDAV_VERSION ${PIM_VERSION})

set(KF5_MIN_VERSION "5.64.0")

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
   add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x060000)
   add_definitions(-DKF_DISABLE_DEPRECATED_BEFORE_AND_AT=0x060000)
endif()

add_definitions(-DQT_NO_FOREACH)



ecm_setup_version(${LIBKDAV_VERSION} VARIABLE_PREFIX KDAV
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/kpimkdav_version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KPimKDAVConfigVersion.cmake"
    SOVERSION 5
    )

set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KPimKDAV")

configure_package_config_file(
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

install(FILES kdav.categories  DESTINATION  ${KDE_INSTALL_LOGGINGCATEGORIESDIR})

add_subdirectory(src)
if(BUILD_TESTING)
    add_subdirectory(autotests)
    add_subdirectory(test)
endif()

feature_summary(WHAT ALL
                INCLUDE_QUIET_PACKAGES
                FATAL_ON_MISSING_REQUIRED_PACKAGES
)
