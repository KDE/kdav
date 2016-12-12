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

set( davgroupwareresource_SRCS
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

ecm_qt_declare_logging_category(davgroupwareresource_SRCS HEADER davresource_debug.h IDENTIFIER DAVRESOURCE_LOG CATEGORY_NAME org.kde.pim.davresource)

if(${AccountsQt_FOUND} AND ${SignOnQt_FOUND})
 include_directories(${ACCOUNTSQT_INCLUDE_DIRS} ${SIGNONQT_INCLUDE_DIRS})
 add_definitions(-DHAVE_ACCOUNTS)
 set(davgroupwareresource_SRCS getcredentialsjob.cpp ${davgroupwareresource_SRCS})
endif()


add_library(libdav
   ${davgroupwareresource_SRCS}
   )

target_link_libraries(libdav
       Qt5::Xml
       Qt5::Gui
       Qt5::XmlPatterns
       KF5::KIOCore
       KF5::I18n
       #KF5::AkonadiCore
       #KF5::Contacts
       #KF5::CalendarCore
       )

if(${AccountsQt_FOUND} AND ${SignOnQt_FOUND})
 target_link_libraries(libdav
                     ${ACCOUNTSQT_LIBRARIES}
                     ${SIGNONQT_LIBRARIES})
endif()

install(TARGETS
    libdav
    EXPORT libdavTargets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS} ${LIBRARY_NAMELINK}
    )

#set_target_properties(libdav PROPERTIES
#    VERSION ${LIBDAV_VERSION_STRING}
#    SOVERSION 5
#    EXPORT_NAME libdav
#    )
