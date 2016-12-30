cmake_minimum_required(VERSION 2.8.12)

project(libkdav)

############### KDEPIM-Runtime version ################
# KDEPIM_RUNTIME_VERSION
# Version scheme: "x.y.z build".
#
# x is the version number.
# y is the major release number.
# z is the minor release number.
#
# "x.y.z" follow the kdelibs version kdepim is released with.
#
# If "z" is 0, it the version is "x.y"
#
# KDEPIM_RUNTIME_DEV_VERSION
# is empty for final versions. For development versions "build" is
# something like "pre", "alpha1", "alpha2", "beta1", "beta2", "rc1", "rc2".
#
# Examples in chronological order:
#
#    3.0
#    3.0.1
#    3.1 alpha1
#    3.1 beta1
#    3.1 beta2
#    3.1 rc1
#    3.1
#    3.1.1
#    3.2 pre
#    3.2 alpha1

set(LIBKDAV_VERSION_NUMBER "5.4.0")
set(LIBKDAV_VERSION "5.4.0")

configure_file(libkdav-version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/libkdav-version.h @ONLY)

set(KF5_VERSION "5.27.0")
find_package(ECM ${KF5_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${kdepim-runtime_SOURCE_DIR}/cmake/modules ${ECM_MODULE_PATH})

include(ECMPackageConfigHelpers)
include(ECMSetupVersion)
include(GenerateExportHeader)
include(ECMGenerateHeaders)
include(ECMGeneratePriFile)
include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(ECMInstallIcons)
include(ECMQtDeclareLoggingCategory)
include(ECMCoverageOption)

set(QT_REQUIRED_VERSION "5.6.0")

# QT5 package
find_package(Qt5 ${QT_REQUIRED_VERSION} CONFIG REQUIRED Core Gui XmlPatterns)

find_package(KF5 ${KF5_VERSION} REQUIRED CoreAddons KIO)

add_definitions( -DQT_NO_CAST_FROM_ASCII )
add_definitions( -DQT_NO_CAST_TO_ASCII )
add_definitions( -DQT_NO_URL_CAST_FROM_STRING )

ecm_setup_version(${LIBKDAV_VERSION} VARIABLE_PREFIX KDAV
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/libkdav_version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KDAVConfigVersion.cmake"
    SOVERSION 5
    )


########### Targets ###########

set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KDAV")

ecm_configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/KDAVConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/KDAVConfig.cmake"
    INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
    )

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/KDAVConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/KDAVConfigVersion.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    COMPONENT Devel
    )

install(EXPORT KDAVTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KDAVTargets.cmake NAMESPACE KF5::)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/libkdav_version.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5} COMPONENT Devel
    )


add_subdirectory(src)
add_subdirectory(test)

feature_summary(WHAT ALL
                INCLUDE_QUIET_PACKAGES
                FATAL_ON_MISSING_REQUIRED_PACKAGES
)
