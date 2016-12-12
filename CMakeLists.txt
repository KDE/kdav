cmake_minimum_required(VERSION 2.8.12)

project(libdav)

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

set(LIBDAV_VERSION_NUMBER "5.4.0")
set(LIBDAV_VERSION "5.4.0")

configure_file(libdav-version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/libdav-version.h @ONLY)

set(KF5_VERSION "5.27.0")
find_package(ECM ${KF5_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${kdepim-runtime_SOURCE_DIR}/cmake/modules ${ECM_MODULE_PATH})

include(ECMPackageConfigHelpers)
include(ECMSetupVersion)
include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(ECMInstallIcons)
include(ECMQtDeclareLoggingCategory)
include(ECMCoverageOption)

set(QT_REQUIRED_VERSION "5.6.0")
set(AKONADI_VERSION "5.4.40")

# QT5 package
find_package(Qt5 ${QT_REQUIRED_VERSION} CONFIG REQUIRED Core Gui XmlPatterns)

find_package(KF5KIO ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5I18n ${KF5_VERSION} CONFIG REQUIRED)

# KdepimLibs package
#find_package(KF5Akonadi ${AKONADI_VERSION} CONFIG REQUIRED)

add_definitions( -DQT_NO_CAST_FROM_ASCII )
add_definitions( -DQT_NO_CAST_TO_ASCII )
add_definitions( -DQT_NO_URL_CAST_FROM_STRING )

add_subdirectory(src)
add_subdirectory(test)

feature_summary(WHAT ALL
                INCLUDE_QUIET_PACKAGES
                FATAL_ON_MISSING_REQUIRED_PACKAGES
)
