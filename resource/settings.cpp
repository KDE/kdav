/*
    Copyright (c) 2009 Grégory Oestreicher <greg@kamago.net>
      Based on an original work for the IMAP resource which is :
    Copyright (c) 2008 Volker Krause <vkrause@kde.org>
    Copyright (c) 2008 Omat Holding B.V. <info@omat.nl>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/

#include "settings.h"

#include "settingsadaptor.h"

#include <kapplication.h>
#include <kdebug.h>
#include <kglobal.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kwallet.h>

#include <QtCore/QByteArray>
#include <QtCore/QDataStream>
#include <QtCore/QFile>
#include <QtDBus/QDBusConnection>

class SettingsHelper
{
  public:
    SettingsHelper()
      : q( 0 )
    {
    }

    ~SettingsHelper()
    {
      delete q;
    }

    Settings *q;
};

K_GLOBAL_STATIC( SettingsHelper, s_globalSettings )

Settings::UrlConfiguration::UrlConfiguration()
{
}

Settings::UrlConfiguration::UrlConfiguration( const QString &serialized )
{
  const QStringList splitString = serialized.split( '|' );

  if ( splitString.size() == 3 ) {
    mUrl = splitString.at( 2 );
    mProtocol = DavUtils::protocolByName( splitString.at( 1 ) );
    mUser = splitString.at( 0 );
  }
}

QString Settings::UrlConfiguration::serialize()
{
  QString serialized = mUser;
  serialized.append( '|' ).append( DavUtils::protocolName( DavUtils::Protocol( mProtocol ) ) );
  serialized.append( '|' ).append( mUrl );
  return serialized;
}

Settings *Settings::self()
{
  if ( !s_globalSettings->q ) {
    new Settings;
    s_globalSettings->q->readConfig();
  }

  return s_globalSettings->q;
}

Settings::Settings()
  : SettingsBase(), mWinId( 0 )
{
  Q_ASSERT( !s_globalSettings->q );
  s_globalSettings->q = this;

  new SettingsAdaptor( this );
  QDBusConnection::sessionBus().registerObject( QLatin1String( "/Settings" ), this,
                              QDBusConnection::ExportAdaptors | QDBusConnection::ExportScriptableContents );
}

Settings::~Settings()
{
  QMapIterator<QString, UrlConfiguration*> it( mUrls );
  while ( it.hasNext() ) {
    it.next();
    delete it.value();
  }
}

void Settings::setWinId( WId winId )
{
  mWinId = winId;
}

void Settings::cleanup()
{
  QFile cacheFile( mCollectionsUrlsMappingCache );
  cacheFile.remove();
}

DavUtils::DavUrl::List Settings::configuredDavUrls()
{
  if ( mUrls.isEmpty() ) {
    foreach ( const QString &serializedUrl, remoteUrls() ) {
      UrlConfiguration *urlConfig = new UrlConfiguration( serializedUrl );
      QString key = urlConfig->mUrl + "," + DavUtils::protocolName( DavUtils::Protocol( urlConfig->mProtocol ) );
      urlConfig->mPassword = loadPassword( key, urlConfig->mUser );
      mUrls[ key ] = urlConfig;
    }
  }
  DavUtils::DavUrl::List davUrls;
  QMapIterator<QString, UrlConfiguration*> it( mUrls );

  while ( it.hasNext() ) {
    it.next();
    QStringList split = it.key().split( "," );
    davUrls << configuredDavUrl( DavUtils::protocolByName( split.at( 1 ) ), split.at( 0 ) );
  }

  return davUrls;
}

DavUtils::DavUrl Settings::configuredDavUrl( DavUtils::Protocol proto, const QString &searchUrl, const QString &finalUrl )
{
  KUrl fullUrl;

  if ( !finalUrl.isEmpty() )
    fullUrl = finalUrl;
  else
    fullUrl = searchUrl;

  const QString user = username( proto, searchUrl );
  fullUrl.setUser( user );
  fullUrl.setPassword( password( proto, searchUrl ) );

  return DavUtils::DavUrl( fullUrl, proto );
}

DavUtils::DavUrl Settings::davUrlFromCollectionUrl( const QString &collectionUrl, const QString &finalUrl )
{
  if ( mCollectionsUrlsMapping.isEmpty() )
    loadMappings();

  DavUtils::DavUrl davUrl;
  QString targetUrl = finalUrl.isEmpty() ? collectionUrl : finalUrl;

  if ( mCollectionsUrlsMapping.contains( collectionUrl ) ) {
    QStringList split = mCollectionsUrlsMapping[ collectionUrl ].split( "," );
    if ( split.size() == 2 )
      davUrl = configuredDavUrl( DavUtils::protocolByName( split.at( 1 ) ), split.at( 0 ), targetUrl );
  }

  return davUrl;
}

void Settings::addCollectionUrlMapping( DavUtils::Protocol proto, const QString &collectionUrl, const QString &configuredUrl )
{
  if ( mCollectionsUrlsMapping.isEmpty() )
    loadMappings();

  QString value = configuredUrl + "," + DavUtils::protocolName( proto );
  mCollectionsUrlsMapping.insert( collectionUrl, value );

  // Update the cache now
  QMap<QString, QString> tmp( mCollectionsUrlsMapping );
  QFile cacheFile( mCollectionsUrlsMappingCache );
  if ( cacheFile.open( QIODevice::WriteOnly ) ) {
    QDataStream cache( &cacheFile );
    cache.setVersion( QDataStream::Qt_4_7 );
    cache << mCollectionsUrlsMapping;
    cacheFile.close();
  }
}

QStringList Settings::mappedCollections( DavUtils::Protocol proto, const QString &configuredUrl )
{
  if ( mCollectionsUrlsMapping.isEmpty() )
    loadMappings();

  QString value = configuredUrl + "," + DavUtils::protocolName( proto );
  return mCollectionsUrlsMapping.keys( value );
}

void Settings::newUrlConfiguration( Settings::UrlConfiguration *urlConfig )
{
  QString key = urlConfig->mUrl + "," + DavUtils::protocolName( DavUtils::Protocol( urlConfig->mProtocol ) );

  if ( mUrls.contains( key ) ) {
    removeUrlConfiguration( DavUtils::Protocol( urlConfig->mProtocol ), urlConfig->mUrl );
  }

  mUrls[ key ] = urlConfig;
  savePassword( key, urlConfig->mUser, urlConfig->mPassword );
  updateRemoteUrls();
}

void Settings::removeUrlConfiguration( DavUtils::Protocol proto, const QString &url )
{
  QString key = url + "," + DavUtils::protocolName( proto );

  if ( !mUrls.contains( key ) )
    return;

  delete mUrls[ key ];
  mUrls.remove( key );
  updateRemoteUrls();
}

Settings::UrlConfiguration * Settings::urlConfiguration( DavUtils::Protocol proto, const QString &url )
{
  QString key = url + "," + DavUtils::protocolName( proto );

  UrlConfiguration *ret = 0;
  if ( mUrls.contains( key ) )
    ret = mUrls[ key ];

  return ret;
}

// DavUtils::Protocol Settings::protocol( const QString &url ) const
// {
//   if ( mUrls.contains( url ) )
//     return DavUtils::Protocol( mUrls[ url ]->mProtocol );
//   else
//     return DavUtils::CalDav;
// }

QString Settings::username( DavUtils::Protocol proto, const QString &url ) const
{
  QString key = url + "," + DavUtils::protocolName( proto );

  if ( mUrls.contains( key ) )
    return mUrls[ key ]->mUser;
  else
    return QString();
}

QString Settings::password(DavUtils::Protocol proto, const QString& url)
{
  QString key = url + "," + DavUtils::protocolName( proto );

  if ( mUrls.contains( key ) )
    return mUrls[ key ]->mPassword;
  else
    return QString();
}

void Settings::loadMappings()
{
  QString collectionsMappingCacheBase = QString( "akonadi-davgroupware/%1_c2u.dat" ).arg( KApplication::applicationName() );
  mCollectionsUrlsMappingCache = KStandardDirs::locateLocal( "data", collectionsMappingCacheBase );
  QFile collectionsMappingsCache( mCollectionsUrlsMappingCache );

  if ( collectionsMappingsCache.exists() ) {
    if ( collectionsMappingsCache.open( QIODevice::ReadOnly ) ) {
      QDataStream cache( &collectionsMappingsCache );
      cache >> mCollectionsUrlsMapping;
      collectionsMappingsCache.close();
    }
  }
  else if ( !collectionsUrlsMappings().isEmpty() ) {
    QByteArray rawMappings = QByteArray::fromBase64( collectionsUrlsMappings().toAscii() );
    QDataStream stream( &rawMappings, QIODevice::ReadOnly );
    stream >> mCollectionsUrlsMapping;
    setCollectionsUrlsMappings( QString() );
  }
}

void Settings::updateRemoteUrls()
{
  QStringList newUrls;

  QMapIterator<QString, UrlConfiguration*> it( mUrls );
  while ( it.hasNext() ) {
    it.next();
    newUrls << it.value()->serialize();
  }

  setRemoteUrls( newUrls );
}

void Settings::savePassword(const QString& key, const QString& user, const QString& password)
{
  KWallet::Wallet *wallet = KWallet::Wallet::openWallet( KWallet::Wallet::NetworkWallet(), mWinId );
  if ( !wallet )
    return;

  if ( !wallet->hasFolder( KWallet::Wallet::PasswordFolder() ) )
    wallet->createFolder( KWallet::Wallet::PasswordFolder() );

  if ( !wallet->setFolder( KWallet::Wallet::PasswordFolder() ) )
    return;

  QString entry = key + "," + user;
  wallet->writePassword( entry, password );
}

QString Settings::loadPassword( const QString& key, const QString &user )
{
  KWallet::Wallet *wallet = KWallet::Wallet::openWallet( KWallet::Wallet::NetworkWallet(), mWinId );
  if ( !wallet )
    return QString();

  if ( !wallet->hasFolder( KWallet::Wallet::PasswordFolder() ) )
    wallet->createFolder( KWallet::Wallet::PasswordFolder() );

  if ( !wallet->setFolder( KWallet::Wallet::PasswordFolder() ) )
    return QString();

  QString entry = key + "," + user;
  if ( !wallet->hasEntry( entry ) )
    return QString();

  QString pass;
  wallet->readPassword( entry, pass );
  return pass;
}

#include "settings.moc"
