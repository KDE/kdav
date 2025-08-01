cmake_minimum_required(VERSION 3.16)

set(KF_VERSION "6.18.0") # handled by release scripts
set(KF_DEP_VERSION "6.17.0") # handled by release scripts

project(libkdav VERSION ${KF_VERSION})

find_package(ECM 6.17.0 CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEGitCommitHooks)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)

include(CMakeFindDependencyMacro)
include(ECMSetupVersion)
include(ECMGenerateExportHeader)
include(ECMGenerateHeaders)
include(FeatureSummary)
include(ECMAddTests)
include(ECMInstallIcons)
include(ECMQtDeclareLoggingCategory)
include(ECMDeprecationSettings)
include(ECMGenerateQDoc)


set(REQUIRED_QT_VERSION 6.7.0)
find_package(Qt6 ${REQUIRED_QT_VERSION} CONFIG REQUIRED Core Gui Test)

find_package(KF6 ${KF_DEP_VERSION} REQUIRED CoreAddons KIO I18n)

# setup lib

ecm_set_disabled_deprecation_versions(
    QT 6.10.0
    KF 6.16.0
)

set(kdav_version_header "${CMAKE_CURRENT_BINARY_DIR}/src/kdav_version.h")
ecm_setup_version(PROJECT VARIABLE_PREFIX KDAV
    VERSION_HEADER "${kdav_version_header}"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KF6DAVConfigVersion.cmake"
    SOVERSION 6
    )

ki18n_install(po)

add_subdirectory(src)
if(BUILD_TESTING)
    add_subdirectory(autotests)
    add_subdirectory(test)
endif()

set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KF6DAV")

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/KF6DAVConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/KF6DAVConfig.cmake"
    INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
    )

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/KF6DAVConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/KF6DAVConfigVersion.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    COMPONENT Devel
    )

install(EXPORT KF6DAVTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KF6DAVTargets.cmake NAMESPACE KF6::)

install(FILES
    ${kdav_version_header}
    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF}/KDAV COMPONENT Devel
    )


include(ECMFeatureSummary)
ecm_feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
