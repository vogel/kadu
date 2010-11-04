/*
 * %kadu copyright begin%
 * Copyright 2009, 2009, 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <QTimer>
#include <QRegExp>
#include <QtCrypto>
#include <bsocket.h>
#include <filetransfer.h>
#include <xmpp_tasks.h>

#include "accounts/account.h"
#include "debug.h"

#include "certificates/certificate-helpers.h"
#include "resource/jabber-resource-pool.h"
#include "jabber-account-details.h"
#include "jabber-client.h"
#include "jabber-protocol.h"

#define JABBER_PENALTY_TIME 2

namespace XMPP
{

XMPP::S5BServer *JabberClient::S5bServer = 0L;
QStringList JabberClient::S5bAddressList;
int JabberClient::S5bServerPort = 8010;

JabberClient::JabberClient(JabberProtocol *protocol, QObject *parent) :
		QObject(parent), jabberClient(0), JabberClientStream(0), JabberClientConnector(0),
		JabberTLS(0), JabberTLSHandler(0)/*, privacyManager(0)*/, Protocol(protocol)
{
	cleanUp();

	// initiate penalty timer
	QTimer::singleShot(JABBER_PENALTY_TIME * 1000, this, SLOT(slotUpdatePenaltyTime()));

}

JabberClient::~JabberClient()
{
	if (jabberClient)
		jabberClient->close();

	delete jabberClient;
	delete JabberClientStream;
	delete JabberClientConnector;
	delete JabberTLSHandler;
	delete JabberTLS;

	jabberClient = 0;
	// privacyManager will be deleted with jabberClient, its parent's parent
}

void JabberClient::cleanUp()
{
	if (jabberClient)
		jabberClient->close();

	delete jabberClient;
	delete JabberClientStream;
	delete JabberClientConnector;
	delete JabberTLSHandler;
	delete JabberTLS;
	// privacyManager will be deleted with jabberClient, its parent's parent

	jabberClient = 0L;
	JabberClientStream = 0L;
	JabberClientConnector = 0L;
	JabberTLSHandler = 0L;
	JabberTLS = 0L;
	///privacyManager = 0L;

	CurrentPenaltyTime = 0;

	MyJid = XMPP::Jid();
	Password.clear();

	setForceTLS(false);
	setUseSSL(false);
	setUseXMPP09(false);
	setProbeSSL(false);

	setOverrideHost(false);

	setAllowPlainTextPassword(XMPP::ClientStream::AllowPlainOverTLS);

	setFileTransfersEnabled(false);
	setS5BServerPort(8010);

	setClientName(QString());
	setClientVersion(QString());
	setOSName(QString());

	setTimeZone("UTC", 0);

	setIgnoreTLSWarnings(false);
}

void JabberClient::slotUpdatePenaltyTime()
{
	if (CurrentPenaltyTime >= JABBER_PENALTY_TIME)
		CurrentPenaltyTime -= JABBER_PENALTY_TIME;
	else
		CurrentPenaltyTime = 0;

	QTimer::singleShot(JABBER_PENALTY_TIME * 1000, this, SLOT(slotUpdatePenaltyTime()));
}

bool JabberClient::setS5BServerPort(int port)
{
	S5bServerPort = port;

	if (fileTransfersEnabled())
		return s5bServer()->start(port);

	return true;
}

XMPP::S5BServer *JabberClient::s5bServer()
{
	if (!S5bServer)
	{
		S5bServer = new XMPP::S5BServer();
		QObject::connect(S5bServer, SIGNAL(destroyed()), this, SLOT(slotS5BServerGone()));

		/*
		 * Try to start the server at the default port here.
		 * We have no way of notifying the caller of an error.
		 * However, since the caller will usually also
		 * use setS5BServerPort() to ensure the correct
		 * port, we can return an error code there.
		 */
		if (fileTransfersEnabled())
			s5bServer()->start(S5bServerPort);
	}

	return S5bServer;
}

void JabberClient::slotS5BServerGone()
{
	S5bServer = 0L;

	if (jabberClient)
		jabberClient->s5bManager()->setServer( 0L);
}

void JabberClient::addS5BServerAddress(const QString &address)
{
	QStringList newList;

	S5bAddressList.append(address);

	// now filter the list without dupes
	foreach (QStringList::const_reference str, S5bAddressList)
	{
		if (!newList.contains(str))
			newList.append(str);
	}

	s5bServer()->setHostList(newList);
}

void JabberClient::removeS5BServerAddress(const QString &address)
{
	QStringList newList;

	int idx = S5bAddressList.indexOf( address);

	if (idx != -1)
		S5bAddressList.removeAt(idx);

	if (S5bAddressList.isEmpty())
	{
		delete S5bServer;
		S5bServer = 0L;
	}
	else
	{
		// now filter the list without dupes
		foreach (QStringList::const_reference str, S5bAddressList)
		{
			if (!newList.contains(str))
				newList.append(str);
		}

		s5bServer()->setHostList(newList);
	}
}

void JabberClient::setOverrideHost(bool flag, const QString &server, int port)
{
	OverrideHost = flag;
	Server = server;
	Port = port;
}

void JabberClient::setFileTransfersEnabled(bool flag, const QString &localAddress)
{
	FileTransfersEnabled = flag;
	LocalAddress = localAddress;
}

void JabberClient::setTimeZone(const QString &timeZoneName, int timeZoneOffset)
{
	TimeZoneName = timeZoneName;
	TimeZoneOffset = timeZoneOffset;
}

int JabberClient::getPenaltyTime()
{
	int currentTime = CurrentPenaltyTime;

	CurrentPenaltyTime += JABBER_PENALTY_TIME;

	return currentTime;
}

// PrivacyManager *JabberClient::privacyManager() const
// {
// 	return privacyManager;
// }

void JabberClient::connect(const XMPP::Jid &jid, const QString &password, bool auth)
{
	/*
	 * Close any existing connection.
	 */
	if (jabberClient)
		jabberClient->close();

	MyJid = jid;
	Password = password;

	/*
	 * Return an error if we should force TLS but it's not available.
	 */
	if ((forceTLS() || useSSL() || probeSSL()) && !QCA::isSupported("tls"))
	{
		qDebug("no TLS");
		// no SSL support, at the connecting stage this means the problem is client-side
		emit connectionError(tr("SSL support could not be initialized for account %1. This is most likely because the QCA TLS plugin is not installed on your system."));
		return;
	}

	/*
	 * Instantiate connector, responsible for dealing with the socket.
	 * This class uses KDE's socket code, which in turn makes use of
	 * the global proxy settings.
	 */
	JabberClientConnector = new XMPP::AdvancedConnector;

	JabberClientConnector->setOptSSL(useSSL());

	if (overrideHost())
		JabberClientConnector->setOptHostPort(Server, Port);

	if (useXMPP09())
		JabberClientConnector->setOptProbe(probeSSL());

	AccountProxySettings proxy = Protocol->account().proxySettings();
	if (proxy.enabled())
	{
		XMPP::AdvancedConnector::Proxy proxySettings;

		proxySettings.setHttpConnect(proxy.address(), proxy.port());
		if (proxy.requiresAuthentication())
			proxySettings.setUserPass(proxy.user(), proxy.password());

		JabberClientConnector->setProxy(proxySettings);
	}

	/*
	 * Setup authentication layer
	 */
	if ((forceTLS() || useSSL()) && QCA::isSupported("tls"))
	{
		JabberTLS = new QCA::TLS;
		JabberTLS->setTrustedCertificates(CertificateHelpers::allCertificates(CertificateHelpers::getCertificateStoreDirs()));
		JabberTLSHandler = new QCATLSHandler(JabberTLS);
		JabberTLSHandler->setXMPPCertCheck(true);

		QObject::connect(JabberTLSHandler, SIGNAL(tlsHandshaken()), SLOT(slotTLSHandshaken()));
	}

	/*
	 * Instantiate client stream which handles the network communication by referring
	 * to a connector (proxying etc.) and a TLS handler (security layer)
	 */
	JabberClientStream = new XMPP::ClientStream(JabberClientConnector, JabberTLSHandler);

	{
		using namespace XMPP;
		QObject::connect(JabberClientStream, SIGNAL(needAuthParams(bool, bool, bool)),
				   this, SLOT(slotCSNeedAuthParams(bool, bool, bool)));
		QObject::connect(JabberClientStream, SIGNAL(authenticated()),
				   this, SLOT(slotCSAuthenticated()));
		QObject::connect(JabberClientStream, SIGNAL(connectionClosed()),
				   this, SLOT(slotCSDisconnected()));
		QObject::connect(JabberClientStream, SIGNAL(delayedCloseFinished()),
				   this, SLOT(slotCSDisconnected()));
		QObject::connect(JabberClientStream, SIGNAL(warning(int)),
				   this, SLOT(slotCSWarning(int)));
		QObject::connect(JabberClientStream, SIGNAL(error(int)),
				   this, SLOT(slotCSError(int)));
	}

	JabberClientStream->setOldOnly(useXMPP09());

	/*
	 * Initiate anti-idle timer (will be triggered every 55 seconds).
	 */
	JabberClientStream->setNoopTime(55000);

	/*
	 * Allow plaintext password authentication or not?
	 */
	JabberClientStream->setAllowPlain(allowPlainTextPassword());

	/*
	 * Setup client layer.
	 */
	jabberClient = new XMPP::Client(this);

	/*
	 * Setup privacy manager
	 */
	///privacyManager = new PrivacyManager ( rootTask() );

	/*
	 * Enable file transfer (IP and server will be set after connection
	 * has been established.
	 */
	if (fileTransfersEnabled())
	{
		jabberClient->setFileTransferEnabled(true);
		{
			using namespace XMPP;
			QObject::connect(jabberClient->fileTransferManager(), SIGNAL(incomingReady()),
					   this, SLOT(slotIncomingFileTransfer()));
		}
	}

	/* This should only be done here to connect the signals, otherwise it is a
	 * bad idea.
	 */
	{
		using namespace XMPP;
		QObject::connect(jabberClient, SIGNAL(subscription(const Jid &, const QString &, const QString &)),
				   this, SLOT(slotSubscription(const Jid &, const QString &, const QString &)));
		QObject::connect(jabberClient, SIGNAL(rosterRequestFinished(bool, int, const QString &)),
				   this, SLOT(slotRosterRequestFinished(bool, int, const QString &)));
		QObject::connect(jabberClient, SIGNAL(rosterItemAdded(const RosterItem &)),
				   this, SLOT(slotNewContact(const RosterItem &)));
		QObject::connect(jabberClient, SIGNAL(rosterItemUpdated(const RosterItem &)),
				   this, SLOT(slotContactUpdated(const RosterItem &)));
		QObject::connect(jabberClient, SIGNAL(rosterItemRemoved(const RosterItem &)),
				   this, SLOT(slotContactDeleted(const RosterItem &)));
		QObject::connect(jabberClient, SIGNAL(resourceAvailable(const Jid &, const Resource &)),
				   this, SLOT(slotResourceAvailable(const Jid &, const Resource &)));
		QObject::connect(jabberClient, SIGNAL(resourceUnavailable(const Jid &, const Resource &)),
				   this, SLOT(slotResourceUnavailable(const Jid &, const Resource &)));
		QObject::connect(jabberClient, SIGNAL(messageReceived(const Message &)),
				   this, SLOT(slotReceivedMessage(const Message &)));
		QObject::connect(jabberClient, SIGNAL(groupChatJoined(const Jid &)),
				   this, SLOT(slotGroupChatJoined(const Jid &)));
		QObject::connect(jabberClient, SIGNAL(groupChatLeft(const Jid &)),
				   this, SLOT(slotGroupChatLeft(const Jid &)));
		QObject::connect(jabberClient, SIGNAL(groupChatPresence(const Jid &, const Status &)),
				   this, SLOT(slotGroupChatPresence(const Jid &, const Status &)));
		QObject::connect(jabberClient, SIGNAL(groupChatError(const Jid &, int, const QString &)),
				   this, SLOT(slotGroupChatError(const Jid &, int, const QString &)));
		//QObject::connect(jabberClient, SIGNAL(debugText(const QString &)),
		//		   this, SLOT(slotPsiDebug(const QString &)));
		QObject::connect(jabberClient, SIGNAL(xmlIncoming(const QString&)),
				   this, SLOT(slotIncomingXML(const QString &)));
		QObject::connect(jabberClient, SIGNAL(xmlOutgoing(const QString&)),
				   this, SLOT(slotOutgoingXML(const QString &)));
	}

	jabberClient->setClientName(clientName());
	jabberClient->setClientVersion(clientVersion());
	jabberClient->setOSName(osName());

	// Set caps information
	jabberClient->setCapsNode(capsNode());
	jabberClient->setCapsVersion(capsVersion());

	// Set Disco Identity
	//jabberClient->setIdentity( discoIdentity());


	DiscoItem::Identity identity;
	identity.category = "client";
	identity.type = "pc";
	identity.name = "Kadu";
	jabberClient->setIdentity(identity);

	QStringList features;
//	features << "http://jabber.org/protocol/commands";
	features << "http://jabber.org/protocol/rosterx";
//	features << "http://jabber.org/protocol/muc";
	features << "jabber:x:data";
	jabberClient->setFeatures(Features(features));

	jabberClient->setTimeZone(timeZoneName(), timeZoneOffset());

	jabberClient->connectToServer(JabberClientStream, jid, auth);
}

void JabberClient::disconnect()
{
	if (jabberClient)
		jabberClient->close();
	else
		cleanUp();
}

void JabberClient::disconnect( XMPP::Status &reason)
{
	if (jabberClient)
	{
		if (JabberClientStream->isActive())
		{
			XMPP::JT_Presence *pres = new JT_Presence(rootTask());
			reason.setIsAvailable( false);
			pres->pres( reason);
			pres->go();

			JabberClientStream->close();
			jabberClient->close();
		}
	}
	else
		cleanUp();
}

bool JabberClient::isConnected() const
{
	if (jabberClient)
		return jabberClient->isActive();

	return false;
}

void JabberClient::joinGroupChat(const QString &host, const QString &room, const QString &nick)
{
	client()->groupChatJoin(host, room, nick);
}

void JabberClient::joinGroupChat(const QString &host, const QString &room, const QString &nick, const QString &password)
{
	client()->groupChatJoin(host, room, nick, password);
}

void JabberClient::leaveGroupChat(const QString &host, const QString &room)
{
	client()->groupChatLeave(host, room);
}

void JabberClient::setGroupChatStatus( const QString &host, const QString &room, const XMPP::Status &status)
{
	client()->groupChatSetStatus( host, room, status);
}

void JabberClient::changeGroupChatNick( const QString &host, const QString &room, const QString &nick, const XMPP::Status &status)
{
	client()->groupChatChangeNick( host, room, nick, status);
}

void JabberClient::sendMessage(const XMPP::Message &message)
{
	XMPP::Message m = message;
	emit messageAboutToSend(m);
	client()->sendMessage(m);
}

void JabberClient::send(const QString &packet)
{
	client()->send(packet);
}

void JabberClient::requestRoster()
{
	client()->rosterRequest();
}

void JabberClient::slotPsiDebug(const QString &_msg)
{
	QString msg = _msg;

	msg = msg.replace( QRegExp( "<password>[^<]*</password>\n"), "<password>[Filtered]</password>\n");
	msg = msg.replace( QRegExp( "<digest>[^<]*</digest>\n"), "<digest>[Filtered]</digest>\n");

	emit debugMessage("Psi: " + msg);

}

void JabberClient::slotIncomingXML(const QString &_msg)
{
	QString msg = _msg;

	msg = msg.replace( QRegExp( "<password>[^<]*</password>\n"), "<password>[Filtered]</password>\n");
	msg = msg.replace( QRegExp( "<digest>[^<]*</digest>\n"), "<digest>[Filtered]</digest>\n");

	emit debugMessage("XML IN: " + msg);
	emit incomingXML(msg);
}

void JabberClient::slotOutgoingXML(const QString &_msg)
{
	QString msg = _msg;

	msg = msg.replace( QRegExp( "<password>[^<]*</password>\n"), "<password>[Filtered]</password>\n");
	msg = msg.replace( QRegExp( "<digest>[^<]*</digest>\n"), "<digest>[Filtered]</digest>\n");

	emit debugMessage("XML OUT: " + msg);
	emit outgoingXML(msg);
}

void JabberClient::slotTLSHandshaken()
{
	emit debugMessage("TLS handshake done, testing certificate validity...");

	JabberAccountDetails *jabberAccountDetails = dynamic_cast<JabberAccountDetails *>(Protocol->account().details());
	if (!jabberAccountDetails)
		return;

	QString domain = jabberAccountDetails->tlsOverrideDomain();
	QByteArray cert = jabberAccountDetails->tlsOverrideCert();
	if (CertificateHelpers::checkCertificate(JabberTLS, JabberTLSHandler, domain,
		QString("%1: ").arg(Protocol->account().accountIdentity().name()) + tr("Server Authentication"), XMPP::Jid(Protocol->account().id()).domain(), Protocol->account()))
		JabberTLSHandler->continueAfterHandshake();
	else
		disconnect();
}

void JabberClient::slotCSNeedAuthParams(bool user, bool pass, bool realm)
{
	emit debugMessage("Sending auth credentials...");

	if (user)
		JabberClientStream->setUsername(jid().node());

	if (pass)
		JabberClientStream->setPassword(Password);

	if (realm)
		JabberClientStream->setRealm(jid().domain());

	JabberClientStream->continueAfterParams();

}

void JabberClient::slotCSAuthenticated()
{
	emit debugMessage("Connected to Jabber server.");

	JabberClientConnector->changePollInterval(10); // for http poll, slow down after login

	// Update our jid(if necessary)
	if (!JabberClientStream->jid().isEmpty())
		MyJid = JabberClientStream->jid().bare();

	// get IP address
	ByteStream *bs = JabberClientConnector ? JabberClientConnector->stream() : 0;
	if (!bs)
		return;

	if (bs->inherits("BSocket") || bs->inherits("XMPP::BSocket"))
		LocalAddress =((BSocket *)bs)->address().toString();

	if (fileTransfersEnabled())
	{
		// setup file transfer
		addS5BServerAddress(localAddress());
		jabberClient->s5bManager()->setServer(s5bServer());
	}

	// start the client operation
	jabberClient->start(jid().domain(), jid().node(), Password, jid().resource());


	if (!JabberClientStream->old())
	{
		XMPP::JT_Session *j = new XMPP::JT_Session(jabberClient->rootTask());
		QObject::connect(j,SIGNAL(finished()),this, SLOT(sessionStart_finished()));
		j->go(true);
	}
	else
		emit connected();
}

void JabberClient::sessionStart_finished()
{
	XMPP::JT_Session *j =(XMPP::JT_Session*)sender();
	if (j->success())
		emit connected();
	else
		slotCSError(-1);
}

void JabberClient::slotCSDisconnected()
{
	/* FIXME:
	 * We should delete the XMPP::Client instance here,
	 * but timers etc prevent us from doing so.(Psi does
	 * not like to be deleted from a slot).
	 */

	emit debugMessage("Disconnected, freeing up file transfer port...");

	// delete local address from S5B server
	removeS5BServerAddress(localAddress());

	emit csDisconnected();
}

void JabberClient::slotCSWarning(int warning)
{
	emit debugMessage("Client stream warning.");

	bool showNoTlsWarning = warning == ClientStream::WarnNoTLS && false/*acc.ssl == UserAccount::SSL_Yes*/;
	bool doCleanupStream = !JabberClientStream || showNoTlsWarning;

	if (doCleanupStream)
	{
		disconnect();
		//v_isActive = false;
		//loginStatus = Status(Status::Offline);
		//stateChanged();
		//disconnected();
	}

	if (showNoTlsWarning)
	{
		emit connectionError(tr("The server does not support TLS encryption."));
	}
	else if (!doCleanupStream)
	{
		Q_ASSERT(JabberClientStream);
		JabberClientStream->continueAfterWarning();
	}
}

void JabberClient::slotCSError(int error)
{
	emit debugMessage("Client stream error.");
	QString errorText;
	bool reconn;

	if ((error == XMPP::ClientStream::ErrAuth)
		&& (clientStream()->errorCondition() == XMPP::ClientStream::NotAuthorized))
	{
		kdebug("Incorrect password, retrying.\n");
		Protocol->logout(/*Kopete::Account::BadPassword*/);
		emit connectionError(tr("Incorrect password"));
	}
	else
	{
		//Kopete::Account::DisconnectReason errorClass =  Kopete::Account::Unknown;

		kdebug("Disconnecting.\n");
		// display message to user
		// when removing or disconnecting, connection errors are normal
		if (/*!m_removing && */Protocol->isConnected() || Protocol->isConnecting())
		{
			getErrorInfo(error, JabberClientConnector, JabberClientStream, JabberTLSHandler, &errorText, &reconn);
			if (reconn)
				Protocol->connectToServer();

			emit connectionError(tr("There was an error communicating with the server.\nDetails: %1").arg(errorText));
		}
		if (Protocol->isConnected() || Protocol->isConnecting())
			Protocol->logout(/* errorClass */);

		Protocol->resourcePool()->clear();
	}

}

void JabberClient::addContact(const XMPP::Jid &j, const QString &name, const QStringList &groups, bool authReq)
{
	if (AddedContacts.contains(j.bare()))
		return;

	JT_Roster *r = new JT_Roster(jabberClient->rootTask());
	r->set(j, name, groups);
	r->go(true);

	AddedContacts.append(j.bare());

	if(authReq)
		requestSubscription(j);
}

void JabberClient::removeContact(const XMPP::Jid &j)
{
	if (!jabberClient)
		return;

	AddedContacts.removeAll(j.bare());

	JT_Roster *r = new JT_Roster(jabberClient->rootTask());
	r->remove(j);
	r->go(true);

	//TODO in the future...
	// if it looks like a transport, unregister (but not if it is the server!!)
	/*if(u->isTransport() && !Jid(d->client->host()).compare(u->jid())) {
		JT_UnRegister *ju = new JT_UnRegister(d->client->rootTask());
		ju->unreg(j);
		ju->go(true);
	}
	*/
}

void JabberClient::updateContact(const XMPP::Jid &j, const QString &name, const QStringList &groups)
{
	if (!jabberClient)
		return;

	JT_Roster *r = new JT_Roster(jabberClient->rootTask());
	r->set(j, name, groups);
	r->go(true);
}

void JabberClient::setPresence(const XMPP::Status &status)
{
	kdebug("Status: %s, Reason: %s\n", status.show().toLocal8Bit().data(), status.status().toLocal8Bit().data());

	XMPP::Status newStatus = status;

	// TODO: Check if Caps is enabled
	// Send entity capabilities
	newStatus.setCapsNode(capsNode());
	newStatus.setCapsVersion(capsVersion());
	newStatus.setCapsExt(capsExt());

	JabberAccountDetails *jabberAccountDetails = dynamic_cast<JabberAccountDetails *>(Protocol->account().details());
	if (jabberAccountDetails)
	{
		newStatus.setPriority(jabberAccountDetails->priority());

		XMPP::Resource newResource(jabberAccountDetails->resource(), newStatus);

		// update our resource in the resource pool
		Protocol->resourcePool()->addResource(jid(), newResource);

		// make sure that we only consider our own resource locally
		Protocol->resourcePool()->lockToResource(jid(), newResource);
	}

	/*
	 * Unless we are in the connecting status, send a presence packet to the server
	 */
	if (status.show() != QString("connecting"))
	{
		/*
		 * Make sure we are actually connected before sending out a packet.
		 */
		if (isConnected())
		{
			kdebug("Sending new presence to the server.");

			XMPP::JT_Presence * task = new XMPP::JT_Presence(rootTask());
			task->pres(newStatus);
			task->go(true);
		}
		else
		{
			kdebug("We were not connected, presence update aborted.");
		}
	}

}


void JabberClient::requestSubscription(const XMPP::Jid &jid)
{
	changeSubscription(jid, "subscribe");
}

void JabberClient::resendSubscription(const XMPP::Jid &jid)
{
	changeSubscription(jid, "subscribed");
}

void JabberClient::rejectSubscription(const XMPP::Jid &jid)
{
	changeSubscription(jid, "unsubscribed");
}


void JabberClient::changeSubscription(const XMPP::Jid &jid, const QString &type)
{
	XMPP::JT_Presence *task = new XMPP::JT_Presence(jabberClient->rootTask());
	task->sub(jid, type);
	task->go(true);
}

void JabberClient::slotRosterRequestFinished(bool success, int /*statusCode*/, const QString &/*statusString*/)
{
	emit rosterRequestFinished(success);
}

void JabberClient::slotIncomingFileTransfer()
{
	emit incomingFileTransfer();
}

void JabberClient::slotNewContact(const XMPP::RosterItem &item)
{
	emit newContact(item);
}

void JabberClient::slotContactDeleted(const RosterItem &item)
{
	emit contactDeleted(item);
}

void JabberClient::slotContactUpdated(const RosterItem &item)
{
	emit contactUpdated(item);
}

void JabberClient::slotResourceAvailable(const XMPP::Jid &jid, const Resource &resource)
{
	emit resourceAvailable(jid, resource);
}

void JabberClient::slotResourceUnavailable(const XMPP::Jid &jid, const Resource &resource)
{
	emit resourceUnavailable(jid, resource);
}

void JabberClient::slotReceivedMessage(const Message &message)
{
	emit messageReceived(message);
}

void JabberClient::slotGroupChatJoined(const XMPP::Jid &jid)
{
	emit groupChatJoined(jid);
}

void JabberClient::slotGroupChatLeft(const XMPP::Jid &jid)
{
	emit groupChatLeft(jid);
}

void JabberClient::slotGroupChatPresence(const XMPP::Jid &jid, const Status &status)
{
	emit groupChatPresence(jid, status);
}

void JabberClient::slotGroupChatError(const XMPP::Jid &jid, int error, const QString &reason)
{
	emit groupChatError(jid, error, reason);
}

void JabberClient::slotSubscription(const XMPP::Jid &jid, const QString &type, const QString &nick)
{
	emit subscription(jid, type, nick);
}

void JabberClient::getErrorInfo(int err, AdvancedConnector *conn, Stream *stream, QCATLSHandler *tlsHandler, QString *_str, bool *_reconn)
{
	QString str;
	bool reconn = false;

	if (err == -1)
	{
		str = tr("Disconnected");
		reconn = true;
	}
	else if (err == XMPP::ClientStream::ErrParse)
	{
		str = tr("XML Parsing Error");
		reconn = true;
	}
	else if (err == XMPP::ClientStream::ErrProtocol)
	{
		str = tr("XMPP Protocol Error");
		reconn = true;
	}
	else if (err == XMPP::ClientStream::ErrStream)
	{
		int x;
		QString s, detail;
		reconn = true;
		if (stream)  // Stream can apparently be gone if you disconnect in time
		{
			x = stream->errorCondition();
			detail = stream->errorText();
		}
		else
		{
			x = XMPP::Stream::GenericStreamError;
			reconn = false;
		}

		if (x == XMPP::Stream::GenericStreamError)
			s = tr("Generic stream error");
		else if (x == XMPP::ClientStream::Conflict)
		{
			s = tr("Conflict(remote login replacing this one)");
			reconn = false;
		}
		else if (x == XMPP::ClientStream::ConnectionTimeout)
			s = tr("Timed out from inactivity");
		else if (x == XMPP::ClientStream::InternalServerError)
			s = tr("Internal server error");
		else if (x == XMPP::ClientStream::InvalidXml)
			s = tr("Invalid XML");
		else if (x == XMPP::ClientStream::PolicyViolation)
		{
			s = tr("Policy violation");
			reconn = false;
		}
		else if (x == XMPP::ClientStream::ResourceConstraint)
		{
			s = tr("Server out of resources");
			reconn = false;
		}
		else if (x == XMPP::ClientStream::SystemShutdown)
		{
			s = tr("Server is shutting down");
		}
		str = tr("XMPP Stream Error: %1").arg(s) + '\n' + detail;
	}
	else if (err == XMPP::ClientStream::ErrConnection)
	{
		int x = conn->errorCode();
		QString s;
		reconn = true;
		if (x == XMPP::AdvancedConnector::ErrConnectionRefused)
			s = tr("Unable to connect to server");
		else if (x == XMPP::AdvancedConnector::ErrHostNotFound)
			s = tr("Host not found");
		else if (x == XMPP::AdvancedConnector::ErrProxyConnect)
			s = tr("Error connecting to proxy");
		else if (x == XMPP::AdvancedConnector::ErrProxyNeg)
			s = tr("Error during proxy negotiation");
		else if (x == XMPP::AdvancedConnector::ErrProxyAuth)
		{
			s = tr("Proxy authentication failed");
			reconn = false;
		}
		else if (x == XMPP::AdvancedConnector::ErrStream)
			s = tr("Socket/stream error");
		str = tr("Connection Error: %1").arg(s);
	}
	else if (err == XMPP::ClientStream::ErrNeg)
	{
		QString s, detail;
		int x = stream->errorCondition();
		detail = stream->errorText();
		if (x  == XMPP::ClientStream::HostGone)
			s = tr("Host no longer hosted");
		else if (x == XMPP::ClientStream::HostUnknown)
			s = tr("Host unknown");
		else if (x == XMPP::ClientStream::RemoteConnectionFailed)
		{
			s = tr("A required remote connection failed");
			reconn = true;
		}
		else if (x == XMPP::ClientStream::SeeOtherHost)
			s = tr("See other host: %1").arg(stream->errorText());
		else if (x == XMPP::ClientStream::UnsupportedVersion)
			s = tr("Server does not support proper XMPP version");
		str = tr("Stream Negotiation Error: %1").arg(s) + '\n' + detail;
	}
	else if (err == XMPP::ClientStream::ErrTLS)
	{
		int x = stream->errorCondition();
		QString s;
		if (x == XMPP::ClientStream::TLSStart)
			s = tr("Server rejected STARTTLS");
		else if (x == XMPP::ClientStream::TLSFail)
		{
			int t = tlsHandler->tlsError();
			if (t == QCA::TLS::ErrorHandshake)
				s = tr("TLS handshake error");
			else
				s = tr("Broken security layer (TLS)");
		}
		str = s;
	}
	else if (err == XMPP::ClientStream::ErrAuth)
	{
		int x = stream->errorCondition();
		QString s;
		if (x == XMPP::ClientStream::GenericAuthError)
			s = tr("Unable to login");
		else if (x == XMPP::ClientStream::NoMech)
		{
			s = tr("No appropriate mechanism available for given security settings(e.g. SASL library too weak, or plaintext authentication not enabled)");
			s += '\n' + stream->errorText();
		}
		else if (x == XMPP::ClientStream::BadProto)
			s = tr("Bad server response");
		else if (x == XMPP::ClientStream::BadServ)
			s = tr("Server failed mutual authentication");
		else if (x == XMPP::ClientStream::EncryptionRequired)
			s = tr("Encryption required for chosen SASL mechanism");
		else if (x == XMPP::ClientStream::InvalidAuthzid)
			s = tr("Invalid account information");
		else if (x == XMPP::ClientStream::InvalidMech)
			s = tr("Invalid SASL mechanism");
		else if (x == XMPP::ClientStream::InvalidRealm)
			s = tr("Invalid realm");
		else if (x == XMPP::ClientStream::MechTooWeak)
			s = tr("SASL mechanism too weak for this account");
		else if (x == XMPP::ClientStream::NotAuthorized)
			s = tr("Not authorized");
		else if (x == XMPP::ClientStream::TemporaryAuthFailure)
			s = tr("Temporary auth failure");

		str = tr("Authentication error: %1").arg(s);
	}
	else if (err == XMPP::ClientStream::ErrSecurityLayer)
		str = tr("Broken security layer (SASL)");
	else
		str = tr("None");
	//printf("str[%s], reconn=%d\n", str.latin1(), reconn);
	*_str = str;
	*_reconn = reconn;
}

}
