cmake_minimum_required(VERSION 3.16)

set(KF_VERSION "5.103.0") # handled by release scripts
set(KF_DEP_VERSION "5.103.0") # handled by release scripts

project(libkdav VERSION ${KF_VERSION})

find_package(ECM 5.103.0 CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEGitCommitHooks)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)

include(CMakeFindDependencyMacro)
include(ECMSetupVersion)
include(GenerateExportHeader)
include(ECMGenerateHeaders)
include(ECMGeneratePriFile)
include(FeatureSummary)
include(ECMAddQch)
include(ECMAddTests)
include(ECMInstallIcons)
include(ECMQtDeclareLoggingCategory)
include(ECMDeprecationSettings)


set(REQUIRED_QT_VERSION 5.15.2)
find_package(Qt${QT_MAJOR_VERSION} ${REQUIRED_QT_VERSION} CONFIG REQUIRED Core Gui Test)

find_package(KF5 ${KF_DEP_VERSION} REQUIRED CoreAddons KIO I18n)

# setup lib

option(BUILD_QCH "Build API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)" OFF)
add_feature_info(QCH ${BUILD_QCH} "API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)")

ecm_set_disabled_deprecation_versions(
    QT 5.15.2
    KF 5.95
)

ecm_setup_version(PROJECT VARIABLE_PREFIX KDAV
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/kdav_version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KF5DAVConfigVersion.cmake"
    SOVERSION 5
    )

ki18n_install(po)

add_subdirectory(src)
if(BUILD_TESTING)
    add_subdirectory(autotests)
    add_subdirectory(test)
endif()

set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KF5DAV")

if (BUILD_QCH)
    ecm_install_qch_export(
        TARGETS KF5DAV_QCH
        FILE KF5DAVQchTargets.cmake
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel
    )
    set(PACKAGE_INCLUDE_QCHTARGETS "include(\"\${CMAKE_CURRENT_LIST_DIR}/KF5DAVQchTargets.cmake\")")
endif()

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
    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF}/KDAV COMPONENT Devel
    )


feature_summary(WHAT ALL
                INCLUDE_QUIET_PACKAGES
                FATAL_ON_MISSING_REQUIRED_PACKAGES
)

kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
