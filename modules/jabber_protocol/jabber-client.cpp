
/***************************************************************************
                jabberclient.cpp - Generic Jabber Client Class
                             -------------------
    begin                : Sat May 25 2005
    copyright            : (C) 2005 by Till Gerken <till@tantalo.net>
                           (C) 2006 by Michaël Larouche <larouche@kde.org>
    Copyright 2006 by Tommi Rantala <tommi.rantala@cs.helsinki.fi>

			   Kopete (C) 2001-2006 Kopete developers
			   <kopete-devel@kde.org>.
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "jabber-client.h"
#include "jabber_protocol.h"
#include "debug.h"

#include <QtGui/QMessageBox>
#include <QTimer>
#include <QRegExp>
#include <QtCrypto>

#include <bsocket.h>
#include <filetransfer.h>
#include <jinglesessionmanager.h>
#include <xmpp_tasks.h>

#define JABBER_PENALTY_TIME	2
namespace XMPP {
class JabberClient::Private
{
public:
	Private()
	: jabberClient(0L), jabberClientStream(0L), jabberClientConnector(0L), jabberTLS(0L),
		       jabberTLSHandler(0L)/*, privacyManager(0L)*/
	{}
	~Private()
	{
		if ( jabberClient )
		{
			jabberClient->close ();
		}

		delete jabberClient;
		delete jabberClientStream;
		delete jabberClientConnector;
		delete jabberTLSHandler;
		delete jabberTLS;
		// privacyManager will be deleted with jabberClient, its parent's parent
	}

	// connection details
	XMPP::Jid jid;
	QString password;

	// XMPP backend
	XMPP::Client *jabberClient;
	XMPP::ClientStream *jabberClientStream;
	XMPP::AdvancedConnector *jabberClientConnector;
	QCA::TLS *jabberTLS;
	XMPP::QCATLSHandler *jabberTLSHandler;
	QCA::Initializer qcaInit;
	//PrivacyManager *privacyManager;

	// ignore TLS warnings
	bool ignoreTLSWarnings;

	// current S5B server instance
	static XMPP::S5BServer *s5bServer;
	// address list being handled by the S5B server instance
	static QStringList s5bAddressList;
	// port of S5B server
	static int s5bServerPort;

	// local IP address
	QString localAddress;

	// whether TLS (or direct SSL in case of the old protocol) should be used
	bool forceTLS;

	// whether direct SSL connections should be used
	bool useSSL;

	// use XMPP 1.0 or the older protocol version
	bool useXMPP09;

	// whether SSL support should be probed in case the old protocol is used
	bool probeSSL;

	// override the default server name and port (only pre-XMPP 1.0)
	bool overrideHost;
	QString server;
	int port;

	// allow transmission of plaintext passwords
	XMPP::ClientStream::AllowPlainType allowPlainTextPassword;

	// enable file transfers
	bool fileTransfersEnabled;

	// current penalty time
	int currentPenaltyTime;

	// client information
	QString clientName, clientVersion, osName;

	// timezone information
	QString timeZoneName;
	int timeZoneOffset;

	// Caps(JEP-0115: Entity Capabilities) information
	QString capsNode, capsVersion;
	DiscoItem::Identity discoIdentity;
};

XMPP::S5BServer *JabberClient::Private::s5bServer = 0L;
QStringList JabberClient::Private::s5bAddressList;
int JabberClient::Private::s5bServerPort = 8010;

JabberClient::JabberClient ()
 : d(new Private())
{
	cleanUp ();

	// initiate penalty timer
	QTimer::singleShot ( JABBER_PENALTY_TIME * 1000, this, SLOT ( slotUpdatePenaltyTime () ) );

}

JabberClient::~JabberClient ()
{
	delete d;
}

void JabberClient::cleanUp ()
{
	if ( d->jabberClient )
	{
		d->jabberClient->close ();
	}

	delete d->jabberClient;
	delete d->jabberClientStream;
	delete d->jabberClientConnector;
	delete d->jabberTLSHandler;
	delete d->jabberTLS;
	// privacyManager will be deleted with jabberClient, its parent's parent

	d->jabberClient = 0L;
	d->jabberClientStream = 0L;
	d->jabberClientConnector = 0L;
	d->jabberTLSHandler = 0L;
	d->jabberTLS = 0L;
	///d->privacyManager = 0L;

	d->currentPenaltyTime = 0;

	d->jid = XMPP::Jid ();
	d->password.clear();

	setForceTLS ( false );
	setUseSSL ( false );
	setUseXMPP09 ( false );
	setProbeSSL ( false );

	setOverrideHost ( false );

	setAllowPlainTextPassword(XMPP::ClientStream::AllowPlainOverTLS);

	setFileTransfersEnabled ( false );
	setS5BServerPort ( 8010 );

	setClientName ( QString() );
	setClientVersion ( QString() );
	setOSName ( QString() );

	setTimeZone ( "UTC", 0 );

	setIgnoreTLSWarnings ( false );

}

void JabberClient::slotUpdatePenaltyTime ()
{

	if ( d->currentPenaltyTime >= JABBER_PENALTY_TIME )
		d->currentPenaltyTime -= JABBER_PENALTY_TIME;
	else
		d->currentPenaltyTime = 0;

	QTimer::singleShot ( JABBER_PENALTY_TIME * 1000, this, SLOT ( slotUpdatePenaltyTime () ) );

}

void JabberClient::setIgnoreTLSWarnings ( bool flag )
{

	d->ignoreTLSWarnings = flag;

}

bool JabberClient::ignoreTLSWarnings ()
{

	return d->ignoreTLSWarnings;

}

bool JabberClient::setS5BServerPort ( int port )
{

	d->s5bServerPort = port;

	if ( fileTransfersEnabled () )
	{
		return s5bServer()->start ( port );
	}

	return true;

}

int JabberClient::s5bServerPort () const
{

	return d->s5bServerPort;

}

XMPP::S5BServer *JabberClient::s5bServer ()
{

	if ( !d->s5bServer )
	{
		d->s5bServer = new XMPP::S5BServer ();
		QObject::connect ( d->s5bServer, SIGNAL ( destroyed () ), this, SLOT ( slotS5BServerGone () ) );

		/*
		 * Try to start the server at the default port here.
		 * We have no way of notifying the caller of an error.
		 * However, since the caller will usually also
		 * use setS5BServerPort() to ensure the correct
		 * port, we can return an error code there.
		 */
		if ( fileTransfersEnabled () )
		{
			s5bServer()->start ( d->s5bServerPort );
		}
	}

	return d->s5bServer;

}

void JabberClient::slotS5BServerGone ()
{

	d->s5bServer = 0L;

	if ( d->jabberClient )
		d->jabberClient->s5bManager()->setServer( 0L );

}

void JabberClient::addS5BServerAddress ( const QString &address )
{
	QStringList newList;

	d->s5bAddressList.append ( address );

	// now filter the list without dupes
	foreach( QStringList::const_reference str, d->s5bAddressList )
	{
		if ( !newList.contains ( str ) )
			newList.append ( str );
	}

	s5bServer()->setHostList ( newList );

}

void JabberClient::removeS5BServerAddress ( const QString &address )
{
	QStringList newList;

	int idx = d->s5bAddressList.indexOf( address );

	if ( idx != -1 )
		d->s5bAddressList.removeAt(idx);

	if ( d->s5bAddressList.isEmpty () )
	{
		delete d->s5bServer;
		d->s5bServer = 0L;
	}
	else
	{
		// now filter the list without dupes
		foreach( QStringList::const_reference str, d->s5bAddressList )
		{
			if ( !newList.contains ( str ) )
				newList.append ( str );
		}

		s5bServer()->setHostList ( newList );
	}

}

void JabberClient::setForceTLS ( bool flag )
{

	d->forceTLS = flag;

}

bool JabberClient::forceTLS () const
{

	return d->forceTLS;

}

void JabberClient::setUseSSL ( bool flag )
{

	d->useSSL = flag;

}

bool JabberClient::useSSL () const
{

	return d->useSSL;

}

void JabberClient::setUseXMPP09 ( bool flag )
{

	d->useXMPP09 = flag;

}

bool JabberClient::useXMPP09 () const
{

	return d->useXMPP09;

}

void JabberClient::setProbeSSL ( bool flag )
{

	d->probeSSL = flag;

}

bool JabberClient::probeSSL () const
{

	return d->probeSSL;

}

void JabberClient::setOverrideHost ( bool flag, const QString &server, int port )
{

	d->overrideHost = flag;
	d->server = server;
	d->port = port;

}

bool JabberClient::overrideHost () const
{

	return d->overrideHost;

}

void JabberClient::setAllowPlainTextPassword(XMPP::ClientStream::AllowPlainType flag)
{

	d->allowPlainTextPassword = flag;

}

XMPP::ClientStream::AllowPlainType JabberClient::allowPlainTextPassword () const
{

	return d->allowPlainTextPassword;

}

void JabberClient::setFileTransfersEnabled ( bool flag, const QString &localAddress )
{

	d->fileTransfersEnabled = flag;
	d->localAddress = localAddress;

}

QString JabberClient::localAddress () const
{

	return d->localAddress;

}

bool JabberClient::fileTransfersEnabled () const
{

	return d->fileTransfersEnabled;

}

void JabberClient::setClientName ( const QString &clientName )
{

	d->clientName = clientName;

}

QString JabberClient::clientName () const
{

	return d->clientName;

}

void JabberClient::setClientVersion ( const QString &clientVersion )
{

	d->clientVersion = clientVersion;

}

QString JabberClient::clientVersion () const
{

	return d->clientVersion;

}

void JabberClient::setOSName ( const QString &osName )
{

	d->osName = osName;

}

QString JabberClient::osName () const
{

	return d->osName;

}

void JabberClient::setCapsNode( const QString &capsNode )
{
	d->capsNode = capsNode;
}

QString JabberClient::capsNode() const
{
	return d->capsNode;
}

void JabberClient::setCapsVersion( const QString &capsVersion )
{
	d->capsVersion = capsVersion;
}

QString JabberClient::capsVersion() const
{
	return d->capsVersion;
}

QString JabberClient::capsExt() const
{
	if(d->jabberClient)
	{
		return d->jabberClient->capsExt();
	}

	return QString();
}
void JabberClient::setDiscoIdentity( DiscoItem::Identity identity )
{
	d->discoIdentity = identity;
}

DiscoItem::Identity JabberClient::discoIdentity() const
{
	return d->discoIdentity;
}

void JabberClient::setTimeZone ( const QString &timeZoneName, int timeZoneOffset )
{

	d->timeZoneName = timeZoneName;
	d->timeZoneOffset = timeZoneOffset;

}

QString JabberClient::timeZoneName () const
{

	return d->timeZoneName;

}

int JabberClient::timeZoneOffset () const
{

	return d->timeZoneOffset;

}

int JabberClient::getPenaltyTime ()
{

	int currentTime = d->currentPenaltyTime;

	d->currentPenaltyTime += JABBER_PENALTY_TIME;

	return currentTime;

}

XMPP::Client *JabberClient::client () const
{

	return d->jabberClient;

}

XMPP::ClientStream *JabberClient::clientStream () const
{

	return d->jabberClientStream;

}

XMPP::AdvancedConnector *JabberClient::clientConnector () const
{

	return d->jabberClientConnector;

}

XMPP::Task *JabberClient::rootTask () const
{

	if ( client () )
	{
		return client()->rootTask ();
	}
	else
	{
		return 0l;
	}

}

XMPP::FileTransferManager *JabberClient::fileTransferManager () const
{

	if ( client () )
	{
		return client()->fileTransferManager ();
	}
	else
	{
		return 0L;
	}

}

// PrivacyManager *JabberClient::privacyManager () const
// {
// 	return d->privacyManager;
// }

XMPP::Jid JabberClient::jid () const
{

	return d->jid;

}

void JabberClient::connect ( const XMPP::Jid &jid, const QString &password, bool auth )
{
	/*
	 * Close any existing connection.
	 */
	if ( d->jabberClient )
	{
		d->jabberClient->close ();
	}
	d->jid = jid;
	d->password = password;

	/*
	 * Return an error if we should force TLS but it's not available.
	 */
	if ( ( forceTLS () || useSSL () || probeSSL () ) && !QCA::isSupported ("tls" ) )
	{
		qDebug ("no TLS");
		// no SSL support, at the connecting stage this means the problem is client-side
		QMessageBox* m = new QMessageBox(QMessageBox::Critical, tr("Jabber SSL Error"), tr("SSL support could not be initialized for account %1. This is most likely because the QCA TLS plugin is not installed on your system.").arg(jid.bare()), QMessageBox::Ok, 0, Qt::WDestructiveClose);
		m->setModal(true);
		m->show();
	}

	/*
	 * Instantiate connector, responsible for dealing with the socket.
	 * This class uses KDE's socket code, which in turn makes use of
	 * the global proxy settings.
	 */
	d->jabberClientConnector = new XMPP::AdvancedConnector;

	d->jabberClientConnector->setOptSSL ( useSSL () );

	if ( useXMPP09 () )
	{
		if ( overrideHost () )
		{
			d->jabberClientConnector->setOptHostPort ( d->server, d->port );
		}

		d->jabberClientConnector->setOptProbe ( probeSSL () );

	}

	/*
	 * Setup authentication layer
	 */
	if ( QCA::isSupported ("tls") )
	{
		d->jabberTLS = new QCA::TLS;
		d->jabberTLS->setTrustedCertificates(QCA::systemStore());
		d->jabberTLSHandler = new QCATLSHandler(d->jabberTLS);
		d->jabberTLSHandler->setXMPPCertCheck(true);

		QObject::connect ( d->jabberTLSHandler, SIGNAL ( tlsHandshaken() ), SLOT ( slotTLSHandshaken () ) );
	}

	/*
	 * Instantiate client stream which handles the network communication by referring
	 * to a connector (proxying etc.) and a TLS handler (security layer)
	 */
	d->jabberClientStream = new XMPP::ClientStream ( d->jabberClientConnector, d->jabberTLSHandler );

	{
		using namespace XMPP;
		QObject::connect ( d->jabberClientStream, SIGNAL ( needAuthParams(bool, bool, bool) ),
				   this, SLOT ( slotCSNeedAuthParams (bool, bool, bool) ) );
		QObject::connect ( d->jabberClientStream, SIGNAL ( authenticated () ),
				   this, SLOT ( slotCSAuthenticated () ) );
		QObject::connect ( d->jabberClientStream, SIGNAL ( connectionClosed () ),
				   this, SLOT ( slotCSDisconnected () ) );
		QObject::connect ( d->jabberClientStream, SIGNAL ( delayedCloseFinished () ),
				   this, SLOT ( slotCSDisconnected () ) );
		QObject::connect ( d->jabberClientStream, SIGNAL ( warning (int) ),
				   this, SLOT ( slotCSWarning (int) ) );
		QObject::connect ( d->jabberClientStream, SIGNAL ( error (int) ),
				   this, SLOT ( slotCSError (int) ) );
	}

	d->jabberClientStream->setOldOnly ( useXMPP09 () );

	/*
	 * Initiate anti-idle timer (will be triggered every 55 seconds).
	 */
	d->jabberClientStream->setNoopTime ( 55000 );

	/*
	 * Allow plaintext password authentication or not?
	 */
	d->jabberClientStream->setAllowPlain(allowPlainTextPassword());

	/*
	 * Setup client layer.
	 */
	d->jabberClient = new XMPP::Client ( this );

	/*
	 * Setup privacy manager
	 */
	///d->privacyManager = new PrivacyManager ( rootTask() );

	/*
	 * Enable file transfer (IP and server will be set after connection
	 * has been established.
	 */
	if ( fileTransfersEnabled () )
	{
		d->jabberClient->setFileTransferEnabled ( true );

		{
			using namespace XMPP;
			QObject::connect ( d->jabberClient->fileTransferManager(), SIGNAL ( incomingReady() ),
					   this, SLOT ( slotIncomingFileTransfer () ) );
		}
	}

	/*if (jingleEnabled())
	{*/

#ifdef JINGLE_SUPPORT
		d->jabberClient->setJingleEnabled(true);

		{
			using namespace XMPP;
			QObject::connect ( d->jabberClient->jingleSessionManager(), SIGNAL ( incomingSession() ),
					   this, SLOT ( slotIncomingJingleSession () ) );
		}
#else
		d->jabberClient->setJingleEnabled(false);
#endif
	/*}*/

	/* This should only be done here to connect the signals, otherwise it is a
	 * bad idea.
	 */
	{
		using namespace XMPP;
		QObject::connect ( d->jabberClient, SIGNAL ( subscription (const Jid &, const QString &, const QString &) ),
				   this, SLOT ( slotSubscription (const Jid &, const QString &) ) );
		QObject::connect ( d->jabberClient, SIGNAL ( rosterRequestFinished ( bool, int, const QString & ) ),
				   this, SLOT ( slotRosterRequestFinished ( bool, int, const QString & ) ) );
		QObject::connect ( d->jabberClient, SIGNAL ( rosterItemAdded (const RosterItem &) ),
				   this, SLOT ( slotNewContact (const RosterItem &) ) );
		QObject::connect ( d->jabberClient, SIGNAL ( rosterItemUpdated (const RosterItem &) ),
				   this, SLOT ( slotContactUpdated (const RosterItem &) ) );
		QObject::connect ( d->jabberClient, SIGNAL ( rosterItemRemoved (const RosterItem &) ),
				   this, SLOT ( slotContactDeleted (const RosterItem &) ) );
		QObject::connect ( d->jabberClient, SIGNAL ( resourceAvailable (const Jid &, const Resource &) ),
				   this, SLOT ( slotResourceAvailable (const Jid &, const Resource &) ) );
		QObject::connect ( d->jabberClient, SIGNAL ( resourceUnavailable (const Jid &, const Resource &) ),
				   this, SLOT ( slotResourceUnavailable (const Jid &, const Resource &) ) );
		QObject::connect ( d->jabberClient, SIGNAL ( messageReceived (const Message &) ),
				   this, SLOT ( slotReceivedMessage (const Message &) ) );
		QObject::connect ( d->jabberClient, SIGNAL ( groupChatJoined (const Jid &) ),
				   this, SLOT ( slotGroupChatJoined (const Jid &) ) );
		QObject::connect ( d->jabberClient, SIGNAL ( groupChatLeft (const Jid &) ),
				   this, SLOT ( slotGroupChatLeft (const Jid &) ) );
		QObject::connect ( d->jabberClient, SIGNAL ( groupChatPresence (const Jid &, const Status &) ),
				   this, SLOT ( slotGroupChatPresence (const Jid &, const Status &) ) );
		QObject::connect ( d->jabberClient, SIGNAL ( groupChatError (const Jid &, int, const QString &) ),
				   this, SLOT ( slotGroupChatError (const Jid &, int, const QString &) ) );
		//QObject::connect ( d->jabberClient, SIGNAL (debugText (const QString &) ),
		//		   this, SLOT ( slotPsiDebug (const QString &) ) );
		QObject::connect ( d->jabberClient, SIGNAL ( xmlIncoming(const QString& ) ),
				   this, SLOT ( slotIncomingXML (const QString &) ) );
		QObject::connect ( d->jabberClient, SIGNAL ( xmlOutgoing(const QString& ) ),
				   this, SLOT ( slotOutgoingXML (const QString &) ) );
	}

	d->jabberClient->setClientName ( clientName () );
	d->jabberClient->setClientVersion ( clientVersion () );
	d->jabberClient->setOSName ( osName () );

	// Set caps information
	d->jabberClient->setCapsNode( capsNode() );
	d->jabberClient->setCapsVersion( capsVersion() );

	// Set Disco Identity
	//d->jabberClient->setIdentity( discoIdentity() );


	DiscoItem::Identity identity;
	identity.category = "client";
	identity.type = "pc";
	identity.name = "Kadu";
	d->jabberClient->setIdentity(identity);

	QStringList features;
	features << "http://jabber.org/protocol/commands";
	features << "http://jabber.org/protocol/rosterx";
	features << "http://jabber.org/protocol/muc";
	features << "jabber:x:data";
	d->jabberClient->setFeatures(Features(features));



	d->jabberClient->setTimeZone ( timeZoneName (), timeZoneOffset () );

	d->jabberClient->connectToServer ( d->jabberClientStream, jid, auth );


}

void JabberClient::disconnect ()
{

	if ( d->jabberClient )
	{
		d->jabberClient->close ();
	}
	else
	{
		cleanUp ();
	}

}

void JabberClient::disconnect( XMPP::Status &reason )
{
    if ( d->jabberClient )
    {
        if ( d->jabberClientStream->isActive() )
        {
            XMPP::JT_Presence *pres = new JT_Presence(rootTask());
            reason.setIsAvailable( false );
            pres->pres( reason );
            pres->go();

            d->jabberClientStream->close();
            d->jabberClient->close();
        }
    }
    else
    {
        cleanUp();
    }
}

bool JabberClient::isConnected () const
{

	if ( d->jabberClient )
	{
		return d->jabberClient->isActive ();
	}

	return false;

}

void JabberClient::joinGroupChat ( const QString &host, const QString &room, const QString &nick )
{

	client()->groupChatJoin ( host, room, nick );

}

void JabberClient::joinGroupChat ( const QString &host, const QString &room, const QString &nick, const QString &password )
{
	client()->groupChatJoin ( host, room, nick, password );

}

void JabberClient::leaveGroupChat ( const QString &host, const QString &room )
{

	client()->groupChatLeave ( host, room );

}

void JabberClient::setGroupChatStatus( const QString & host, const QString & room, const XMPP::Status & status )
{
	client()->groupChatSetStatus( host, room, status);
}

void JabberClient::changeGroupChatNick( const QString & host, const QString & room, const QString & nick, const XMPP::Status & status)
{
	client()->groupChatChangeNick( host, room, nick, status );
}


void JabberClient::sendMessage ( const XMPP::Message &message )
{

	client()->sendMessage ( message );

}

void JabberClient::send ( const QString &packet )
{

	client()->send ( packet );

}

void JabberClient::requestRoster ()
{

	client()->rosterRequest ();

}

void JabberClient::slotPsiDebug ( const QString & _msg )
{
	QString msg = _msg;

	msg = msg.replace( QRegExp( "<password>[^<]*</password>\n" ), "<password>[Filtered]</password>\n" );
	msg = msg.replace( QRegExp( "<digest>[^<]*</digest>\n" ), "<digest>[Filtered]</digest>\n" );

	emit debugMessage ( "Psi: " + msg );

}

void JabberClient::slotIncomingXML ( const QString & _msg )
{
	QString msg = _msg;

	msg = msg.replace( QRegExp( "<password>[^<]*</password>\n" ), "<password>[Filtered]</password>\n" );
	msg = msg.replace( QRegExp( "<digest>[^<]*</digest>\n" ), "<digest>[Filtered]</digest>\n" );

	emit debugMessage ( "XML IN: " + msg );
	emit incomingXML ( msg );
}

void JabberClient::slotOutgoingXML ( const QString & _msg )
{
	QString msg = _msg;

	msg = msg.replace( QRegExp( "<password>[^<]*</password>\n" ), "<password>[Filtered]</password>\n" );
	msg = msg.replace( QRegExp( "<digest>[^<]*</digest>\n" ), "<digest>[Filtered]</digest>\n" );

	emit debugMessage ( "XML OUT: " + msg );
	emit outgoingXML ( msg );
}

void JabberClient::slotTLSHandshaken ()
{

	emit debugMessage ( "TLS handshake done, testing certificate validity..." );

	// FIXME: in the future, this should be handled by KDE, not QCA

	QCA::TLS::IdentityResult identityResult = d->jabberTLS->peerIdentityResult();
	QCA::Validity            validityResult = d->jabberTLS->peerCertificateValidity();

	if ( identityResult == QCA::TLS::Valid && validityResult == QCA::ValidityGood )
	{
		emit debugMessage ( "Identity and certificate valid, continuing." );

		// valid certificate, continue
		d->jabberTLSHandler->continueAfterHandshake ();
	}
	else
	{
		emit debugMessage ( "Certificate is not valid, asking user what to do next." );

		// certificate is not valid, query the user
		if ( ignoreTLSWarnings () )
		{
			emit debugMessage ( "We are supposed to ignore TLS warnings, continuing." );
			d->jabberTLSHandler->continueAfterHandshake ();
		}

		emit tlsWarning ( identityResult, validityResult );
	}

}

void JabberClient::continueAfterTLSWarning ()
{

	if ( d->jabberTLSHandler )
	{
		d->jabberTLSHandler->continueAfterHandshake ();
	}

}

void JabberClient::slotCSNeedAuthParams ( bool user, bool pass, bool realm )
{
	emit debugMessage ( "Sending auth credentials..." );

	if ( user )
	{
		d->jabberClientStream->setUsername ( jid().node () );
	}

	if ( pass )
	{
		d->jabberClientStream->setPassword ( d->password );
	}

	if ( realm )
	{
		d->jabberClientStream->setRealm ( jid().domain () );
	}

	d->jabberClientStream->continueAfterParams ();

}

void JabberClient::slotCSAuthenticated ()
{
	emit debugMessage ( "Connected to Jabber server." );

	/*
	 * Determine local IP address.
	 * FIXME: This is ugly!
	 */
	if ( localAddress().isEmpty () )
	{
		// code for Iris-type bytestreams
		ByteStream *irisByteStream = d->jabberClientConnector->stream();
		if ( irisByteStream->inherits ( "BSocket" ) || irisByteStream->inherits ( "XMPP::BSocket" ) )
		{
			d->localAddress = ( (BSocket *)irisByteStream )->address().toString ();
		}

		// code for the KDE-type bytestream
		/*Jabber*/ByteStream *kdeByteStream = dynamic_cast</*Jabber*/ByteStream*>(d->jabberClientConnector->stream());
// 		if ( kdeByteStream )
// 		{
// 			d->localAddress = kdeByteStream->socket()->peerName();
// 		}
	}

	if ( fileTransfersEnabled () )
	{
		// setup file transfer
		addS5BServerAddress ( localAddress () );
		d->jabberClient->s5bManager()->setServer ( s5bServer () );
	}

	// start the client operation
	d->jabberClient->start ( jid().domain (), jid().node (), d->password, jid().resource () );

	emit connected ();
}

void JabberClient::slotCSDisconnected ()
{

	/* FIXME:
	 * We should delete the XMPP::Client instance here,
	 * but timers etc prevent us from doing so. (Psi does
	 * not like to be deleted from a slot).
	 */

	emit debugMessage ( "Disconnected, freeing up file transfer port..." );

	// delete local address from S5B server
	removeS5BServerAddress ( localAddress () );

	emit csDisconnected ();

}

void JabberClient::slotCSWarning ( int warning )
{
	emit debugMessage ( "Client stream warning." );

	bool showNoTlsWarning = warning == ClientStream::WarnNoTLS && false/*d->acc.ssl == UserAccount::SSL_Yes*/;
	bool doCleanupStream = !d->jabberClientStream || showNoTlsWarning;

	if (doCleanupStream) {
		disconnect();
		//v_isActive = false;
		//d->loginStatus = Status(Status::Offline);
		//stateChanged();
		//disconnected();
	}

	if (showNoTlsWarning) {
		QMessageBox* m = new QMessageBox(QMessageBox::Critical, /*(d->psi->contactList()->enabledAccounts().count() > 1 ? QString("%1: ").arg(name()) : "") + */tr("Server Error"), tr("The server does not support TLS encryption."), QMessageBox::Ok, 0, Qt::WDestructiveClose);
		m->setModal(true);
		m->show();
	}
	else if (!doCleanupStream) {
		Q_ASSERT(d->jabberClientStream);
		d->jabberClientStream->continueAfterWarning();
	}
}

void JabberClient::slotCSError ( int error )
{
	emit debugMessage ( "Client stream error." );
	QString errorText;
	bool reconn;

	if ((error == XMPP::ClientStream::ErrAuth)
		&& (clientStream()->errorCondition() == XMPP::ClientStream::NotAuthorized))
	{
		kdebug("Incorrect password, retrying.\n");
		protocol->logout(/*Kopete::Account::BadPassword*/);
	}
	else
	{
		//Kopete::Account::DisconnectReason errorClass =  Kopete::Account::Unknown;

		kdebug("Disconnecting.\n");
		// display message to user
		// when removing or disconnecting, connection errors are normal
		if (/*!m_removing && */protocol->isConnected() || protocol->isConnecting())
		{
			getErrorInfo(error, d->jabberClientConnector, d->jabberClientStream, d->jabberTLSHandler, &errorText, &reconn);
			QMessageBox* m = new QMessageBox(QMessageBox::Critical,
	                                 (/*printAccountName*/1 ? QString("%1: ").arg(name()) : "") + tr("Server Error"),
	                                 tr("There was an error communicating with the server.\nDetails: %1").arg(errorText),
	                                 QMessageBox::Ok, 0, Qt::WDestructiveClose);
			m->setModal(true);
			m->show();
		}
		if (protocol->isConnected() || protocol->isConnecting())
			protocol->logout(/* errorClass */);

		protocol->resourcePool()->clear();
	}
	
}

void JabberClient::slotRosterRequestFinished ( bool success, int /*statusCode*/, const QString &/*statusString*/ )
{

	emit rosterRequestFinished ( success );

}

void JabberClient::slotIncomingFileTransfer ()
{

	emit incomingFileTransfer ();

}

void JabberClient::slotNewContact ( const XMPP::RosterItem &item )
{

	emit newContact ( item );

}

void JabberClient::slotContactDeleted ( const RosterItem &item )
{

	emit contactDeleted ( item );

}

void JabberClient::slotContactUpdated ( const RosterItem &item )
{

	emit contactUpdated ( item );

}

void JabberClient::slotResourceAvailable ( const Jid &jid, const Resource &resource )
{

	emit resourceAvailable ( jid, resource );

}

void JabberClient::slotResourceUnavailable ( const Jid &jid, const Resource &resource )
{

	emit resourceUnavailable ( jid, resource );

}

void JabberClient::slotReceivedMessage ( const Message &message )
{

	emit messageReceived ( message );

}

void JabberClient::slotGroupChatJoined ( const Jid &jid )
{

	emit groupChatJoined ( jid );

}

void JabberClient::slotGroupChatLeft ( const Jid &jid )
{

	emit groupChatLeft ( jid );

}

void JabberClient::slotGroupChatPresence ( const Jid &jid, const Status &status)
{

	emit groupChatPresence ( jid, status );

}

void JabberClient::slotGroupChatError ( const Jid &jid, int error, const QString &reason)
{

	emit groupChatError ( jid, error, reason );

}

void JabberClient::slotSubscription ( const Jid &jid, const QString &type )
{

	emit subscription ( jid, type );

}

void JabberClient::getErrorInfo(int err, AdvancedConnector *conn, Stream *stream, QCATLSHandler *tlsHandler, QString *_str, bool *_reconn)
{
	QString str;
	bool reconn = false;

	if(err == -1) {
		str = tr("Disconnected");
		reconn = true;
	}
	else if(err == XMPP::ClientStream::ErrParse) {
		str = tr("XML Parsing Error");
		reconn = true;
	}
	else if(err == XMPP::ClientStream::ErrProtocol) {
		str = tr("XMPP Protocol Error");
		reconn = true;
	}
	else if(err == XMPP::ClientStream::ErrStream) {
		int x;
		QString s, detail;
		reconn = true;
		if (stream) { // Stream can apparently be gone if you disconnect in time
			x = stream->errorCondition();
			detail = stream->errorText();
		} else {
			x = XMPP::Stream::GenericStreamError;
			reconn = false;
		}

		if(x == XMPP::Stream::GenericStreamError)
			s = tr("Generic stream error");
		else if(x == XMPP::ClientStream::Conflict) {
			s = tr("Conflict (remote login replacing this one)");
			reconn = false;
		}
		else if(x == XMPP::ClientStream::ConnectionTimeout)
			s = tr("Timed out from inactivity");
		else if(x == XMPP::ClientStream::InternalServerError)
			s = tr("Internal server error");
		else if(x == XMPP::ClientStream::InvalidXml)
			s = tr("Invalid XML");
		else if(x == XMPP::ClientStream::PolicyViolation) {
			s = tr("Policy violation");
			reconn = false;
		}
		else if(x == XMPP::ClientStream::ResourceConstraint) {
			s = tr("Server out of resources");
			reconn = false;
		}
		else if(x == XMPP::ClientStream::SystemShutdown) {
			s = tr("Server is shutting down");
		}
		str = tr("XMPP Stream Error: %1").arg(s) + "\n" + detail;
	}
	else if(err == XMPP::ClientStream::ErrConnection) {
		int x = conn->errorCode();
		QString s;
		reconn = true;
		if(x == XMPP::AdvancedConnector::ErrConnectionRefused)
			s = tr("Unable to connect to server");
		else if(x == XMPP::AdvancedConnector::ErrHostNotFound)
			s = tr("Host not found");
		else if(x == XMPP::AdvancedConnector::ErrProxyConnect)
			s = tr("Error connecting to proxy");
		else if(x == XMPP::AdvancedConnector::ErrProxyNeg)
			s = tr("Error during proxy negotiation");
		else if(x == XMPP::AdvancedConnector::ErrProxyAuth) {
			s = tr("Proxy authentication failed");
			reconn = false;
		}
		else if(x == XMPP::AdvancedConnector::ErrStream)
			s = tr("Socket/stream error");
		str = tr("Connection Error: %1").arg(s);
	}
	else if(err == XMPP::ClientStream::ErrNeg) {
		QString s, detail;
		int x = stream->errorCondition();
		detail = stream->errorText();
		if(x == XMPP::ClientStream::HostGone)
			s = tr("Host no longer hosted");
		else if(x == XMPP::ClientStream::HostUnknown)
			s = tr("Host unknown");
		else if(x == XMPP::ClientStream::RemoteConnectionFailed) {
			s = tr("A required remote connection failed");
			reconn = true;
		}
		else if(x == XMPP::ClientStream::SeeOtherHost)
			s = tr("See other host: %1").arg(stream->errorText());
		else if(x == XMPP::ClientStream::UnsupportedVersion)
			s = tr("Server does not support proper XMPP version");
		str = tr("Stream Negotiation Error: %1").arg(s) + "\n" + detail;
	}
	else if(err == XMPP::ClientStream::ErrTLS) {
		int x = stream->errorCondition();
		QString s;
		if(x == XMPP::ClientStream::TLSStart)
			s = tr("Server rejected STARTTLS");
		else if(x == XMPP::ClientStream::TLSFail) {
			int t = tlsHandler->tlsError();
			if(t == QCA::TLS::ErrorHandshake)
				s = tr("TLS handshake error");
			else
				s = tr("Broken security layer (TLS)");
		}
		str = s;
	}
	else if(err == XMPP::ClientStream::ErrAuth) {
		int x = stream->errorCondition();
		QString s;
		if(x == XMPP::ClientStream::GenericAuthError) {
			s = tr("Unable to login");
		} else if(x == XMPP::ClientStream::NoMech) {
			s = tr("No appropriate mechanism available for given security settings (e.g. SASL library too weak, or plaintext authentication not enabled)");
			s += "\n" + stream->errorText();
		} else if(x == XMPP::ClientStream::BadProto) {
			s = tr("Bad server response");
		} else if(x == XMPP::ClientStream::BadServ) {
			s = tr("Server failed mutual authentication");
		} else if(x == XMPP::ClientStream::EncryptionRequired) {
			s = tr("Encryption required for chosen SASL mechanism");
		} else if(x == XMPP::ClientStream::InvalidAuthzid) {
			s = tr("Invalid account information");
		} else if(x == XMPP::ClientStream::InvalidMech) {
			s = tr("Invalid SASL mechanism");
		} else if(x == XMPP::ClientStream::InvalidRealm) {
			s = tr("Invalid realm");
		} else if(x == XMPP::ClientStream::MechTooWeak) {
			s = tr("SASL mechanism too weak for this account");
		} else if(x == XMPP::ClientStream::NotAuthorized) {
			s = tr("Not authorized");
		} else if(x == XMPP::ClientStream::TemporaryAuthFailure) {
			s = tr("Temporary auth failure");
		}
		str = tr("Authentication error: %1").arg(s);
	}
	else if(err == XMPP::ClientStream::ErrSecurityLayer)
		str = tr("Broken security layer (SASL)");
	else
		str = tr("None");
	//printf("str[%s], reconn=%d\n", str.latin1(), reconn);
	*_str = str;
	*_reconn = reconn;
}

}
