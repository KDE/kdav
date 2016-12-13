add_definitions(-DTRANSLATION_DOMAIN=\"akonadi_davgroupware_resource\")

if(WIN32)
   set(LIB_INSTALL_DIR ${KDE_INSTALL_LIBDIR}
                       RUNTIME DESTINATION ${KDE_INSTALL_BINDIR}
                       LIBRARY DESTINATION ${KDE_INSTALL_LIBDIR}
                       ARCHIVE DESTINATION ${KDE_INSTALL_LIBDIR} )
endif()

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
 common/davutils.cpp
 common/etagcache.cpp

 protocols/groupdavprotocol.cpp

 #KContacts
 protocols/carddavprotocol.cpp

 #KCalCore
 protocols/caldavprotocol.cpp

 #With Akonadi reference
 #common/etagcache-akonadi.cpp

 #common/davutils-akonadi.cpp
 #resource/davprotocolattribute.cpp
)

ecm_qt_declare_logging_category(libkdav_SRCS HEADER kdav_debug.h IDENTIFIER KDAV_LOG CATEGORY_NAME org.kde.pim.kdav)

add_library(libkdav
   ${libkdav_SRCS}
   )

generate_export_header(libkdav BASE_NAME libkdav)

target_link_libraries(libkdav
PRIVATE
       Qt5::Xml
       Qt5::Gui
       Qt5::XmlPatterns
       KF5::KIOCore
       KF5::I18n
       #KF5::AkonadiCore
       #KF5::Contacts
       #KF5::CalendarCore
       )


install(TARGETS
    libkdav
    EXPORT KDAVTargets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS} ${LIBRARY_NAMELINK}
    )

#set_target_properties(libkdav PROPERTIES
#    VERSION ${LIBDAV_VERSION_STRING}
#    SOVERSION 5
#    EXPORT_NAME libkdav
#    )
