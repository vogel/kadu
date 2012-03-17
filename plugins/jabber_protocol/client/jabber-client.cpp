/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009, 2009, 2010, 2010, 2011 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QCryptographicHash>
#include <QtCore/QRegExp>
#include <QtCore/QTimer>
#include <QtCore/QUrl>
#include <QtCrypto>

#include <bsocket.h>
#include <xmpp_tasks.h>

#include "accounts/account.h"
#include "identities/identity.h"
#include "network/proxy/network-proxy-manager.h"
#include "debug.h"

#include "certificates/certificate-helpers.h"
#include "client/pong-server.h"
#include "resource/jabber-resource-pool.h"
#include "utils/pep-manager.h"
#include "utils/server-info-manager.h"
#include "jabber-account-details.h"
#include "jabber-client.h"
#include "jabber-protocol.h"

#define JABBER_PENALTY_TIME 2

namespace XMPP
{

JabberClient::JabberClient(JabberProtocol *protocol, QObject *parent) :
		QObject(parent), Client(0), JabberClientStream(0), JabberClientConnector(0),
		JabberTLS(0), JabberTLSHandler(0), Protocol(protocol), serverInfoManager(0), PepManager(0)
{
	cleanUp();

	Client = new XMPP::Client(this);

	updateClientInfo();

	// Set caps information
	Client->setCapsNode(capsNode());

	// Set Disco Identity
	//jabberClient->setIdentity( discoIdentity());

	DiscoItem::Identity identity;
	identity.category = "client";
	identity.type = "pc";
	identity.name = "Kadu";
	Client->setIdentity(identity);

	QStringList features;
	features
			<< "http://jabber.org/protocol/bytestreams"	// file transfer
			<< "http://jabber.org/protocol/chatstates"
			<< "http://jabber.org/protocol/disco#info"
			<< "http://jabber.org/protocol/ibb"	// file transfer
			<< "http://jabber.org/protocol/si"	// file transfer
			<< "http://jabber.org/protocol/si/profile/file-transfer" // file transfer
			<< "jabber:iq:version"
			<< "jabber:x:data"
			<< "urn:xmpp:avatar:data"
			<< "urn:xmpp:avatar:metadata"
			<< "urn:xmpp:avatar:metadata+notify"
			<< "urn:xmpp:ping";

	setCapsVersion(calculateCapsVersion(identity, features));

	Client->setCapsVersion(capsVersion());
	Client->setFeatures(Features(features));

	serverInfoManager = new ServerInfoManager(Client, Client);
	QObject::connect(serverInfoManager, SIGNAL(featuresChanged()),
		this, SLOT(serverFeaturesChanged()));

	PepManager = new PEPManager(Client, serverInfoManager, Client);
	QObject::connect(PepManager, SIGNAL(publish_success(const QString&, const XMPP::PubSubItem&)),
		this, SIGNAL(publishSuccess(const QString&,const XMPP::PubSubItem&)));
	QObject::connect(PepManager, SIGNAL(publish_error(const QString&, const XMPP::PubSubItem&)),
		this, SIGNAL(publishError(const QString&,const XMPP::PubSubItem&)));
	PepAvailable = false;

	new PongServer(Client->rootTask());

	/* This should only be done here to connect the signals, otherwise it is a
	 * bad idea.
	 */
	{
		using namespace XMPP;
		QObject::connect(Client, SIGNAL(subscription(const Jid &, const QString &, const QString &)),
				   this, SLOT(slotSubscription(const Jid &, const QString &, const QString &)));
		QObject::connect(Client, SIGNAL(resourceAvailable(const Jid &, const Resource &)),
				   this, SLOT(slotResourceAvailable(const Jid &, const Resource &)));
		QObject::connect(Client, SIGNAL(resourceUnavailable(const Jid &, const Resource &)),
				   this, SLOT(slotResourceUnavailable(const Jid &, const Resource &)));
		//QObject::connect(jabberClient, SIGNAL(debugText(const QString &)),
		//		   this, SLOT(slotPsiDebug(const QString &)));
		QObject::connect(Client, SIGNAL(xmlIncoming(const QString&)),
				   this, SLOT(slotIncomingXML(const QString &)));
		QObject::connect(Client, SIGNAL(xmlOutgoing(const QString&)),
				   this, SLOT(slotOutgoingXML(const QString &)));
	}

	// initiate penalty timer
	QTimer::singleShot(JABBER_PENALTY_TIME * 1000, this, SLOT(slotUpdatePenaltyTime()));
}

JabberClient::~JabberClient()
{
	if (Client)
		Client->close();

	delete Client;
	delete JabberClientStream;
	delete JabberClientConnector;
	delete JabberTLSHandler;
	delete JabberTLS;

	Client = 0;
	JabberClientStream = 0;
	JabberClientConnector = 0;
	JabberTLSHandler = 0;
	JabberTLS = 0;
}

void JabberClient::cleanUp()
{
	if (Client)
		Client->close();

	delete JabberClientStream;
	delete JabberClientConnector;
	delete JabberTLSHandler;
	delete JabberTLS;

	JabberClientStream = 0;
	JabberClientConnector = 0;
	JabberTLSHandler = 0;
	JabberTLS = 0;

	CurrentPenaltyTime = 0;

	MyJid = XMPP::Jid();
	Password.clear();

	setForceTLS(false);
	setUseSSL(false);
	setUseXMPP09(false);
	setProbeSSL(false);

	setOverrideHost(false);

	setAllowPlainTextPassword(XMPP::ClientStream::AllowPlainOverTLS);

	setClientName(QString());
	setClientVersion(QString());
	setOSName(QString());

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

void JabberClient::setOverrideHost(bool flag, const QString &server, int port)
{
	OverrideHost = flag;
	Server = server;
	Port = port;
}

int JabberClient::getPenaltyTime()
{
	int currentTime = CurrentPenaltyTime;

	CurrentPenaltyTime += JABBER_PENALTY_TIME;

	return currentTime;
}

void JabberClient::updateClientInfo()
{
	Client->setClientName(clientName());
	Client->setClientVersion(clientVersion());
	Client->setOSName(osName());
}

void JabberClient::connect(const XMPP::Jid &jid, const QString &password, bool auth)
{
	MyJid = jid;
	Password = password;

	updateClientInfo();

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

	NetworkProxy proxy = Protocol->account().useDefaultProxy()
			? NetworkProxyManager::instance()->defaultProxy()
			: Protocol->account().proxy();

	if (proxy && !proxy.address().isEmpty())
	{
		XMPP::AdvancedConnector::Proxy proxySettings;

		if (proxy.type() == "http") // HTTP Connect
			proxySettings.setHttpConnect(proxy.address(), proxy.port());
		else if (proxy.type() == "socks") // SOCKS
			proxySettings.setSocks(proxy.address(), proxy.port());
		else if (proxy.type() == "poll") // HTTP Poll
		{
			QUrl pollingUrl = proxy.pollingUrl();
			if (pollingUrl.queryItems().isEmpty())
			{
				if (overrideHost())
				{
					QString host = Server.isEmpty() ? MyJid.domain() : Server;
					pollingUrl.addQueryItem("server", host + ':' + QString::number(Port));
				}
				else
					pollingUrl.addQueryItem("server", MyJid.domain());
			}
			proxySettings.setHttpPoll(proxy.address(), proxy.port(), pollingUrl.toString());
			proxySettings.setPollInterval(2);
		}

		if (!proxy.user().isEmpty())
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

		JabberAccountDetails *jabberAccountDetails = dynamic_cast<JabberAccountDetails *>(Protocol->account().details());
		if (jabberAccountDetails)
		{
			QString host = jabberAccountDetails->useCustomHostPort() ? jabberAccountDetails->customHost() : XMPP::Jid(Protocol->account().id()).domain();
			JabberTLSHandler->startClient(host);
		}

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

	Client->connectToServer(JabberClientStream, jid, auth);
}

void JabberClient::disconnect()
{
	// do real disconnect
	XMPP::Status status = XMPP::Status(XMPP::Status::Offline);
	disconnect(status);
}

void JabberClient::disconnect(XMPP::Status &reason)
{
	Client->setPresence(reason);
	//HACK Following PSI solution, server needs some delay to store status properly before we close the connection.
	QTimer::singleShot(100, this, SLOT(cleanUp()));
}

bool JabberClient::isConnected() const
{
	return Client->isActive();
}

void JabberClient::send(const QString &packet)
{
	Client->send(packet);
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
	QString host = jabberAccountDetails->useCustomHostPort() ? jabberAccountDetails->customHost() : XMPP::Jid(Protocol->account().id()).domain();
	QByteArray cert = jabberAccountDetails->tlsOverrideCert();

	if (CertificateHelpers::checkCertificate(JabberTLS, JabberTLSHandler, domain,
		QString("%1: ").arg(Protocol->account().accountIdentity().name()) + tr("security problem"), host, Protocol))
		JabberTLSHandler->continueAfterHandshake();
	else
		Protocol->logout();
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
		MyJid = JabberClientStream->jid();

	// get IP address
	ByteStream *bs = JabberClientConnector ? JabberClientConnector->stream() : 0;
	if (!bs)
		return;

	if (bs->inherits("BSocket") || bs->inherits("XMPP::BSocket"))
		LocalAddress =((BSocket *)bs)->address().toString();

	// start the client operation
	Client->start(jid().domain(), jid().node(), Password, jid().resource());


	if (!JabberClientStream->old())
	{
		XMPP::JT_Session *j = new XMPP::JT_Session(Client->rootTask());
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

	emit csDisconnected();
}

void JabberClient::slotCSWarning(int warning)
{
	emit debugMessage("Client stream warning.");

	bool showNoTlsWarning = (warning == ClientStream::WarnNoTLS) && forceTLS();
	bool doCleanupStream = !JabberClientStream || showNoTlsWarning;

	if (doCleanupStream)
		Protocol->logout();

	if (showNoTlsWarning)
		emit connectionError(tr("The server does not support TLS encryption."));
	else if (!doCleanupStream)
		JabberClientStream->continueAfterWarning();
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
		Protocol->logout();

		emit Protocol->stateMachinePasswordRequired();
	}
	else
	{
		kdebug("Disconnecting.\n");

		if (Protocol->isConnected() || Protocol->isConnecting())
		{
			getErrorInfo(error, JabberClientConnector, JabberClientStream, JabberTLSHandler, &errorText, &reconn);
			emit connectionError(tr("There was an error communicating with the server.\nDetails: %1").arg(errorText));

			if (reconn)
			{
				cleanUp();
				Protocol->connectionError();
			}
			else
			{
				Protocol->connectionClosed();
				Protocol->resourcePool()->clear();
			}
		}
	}
}

void JabberClient::setPresence(const XMPP::Status &status)
{
	kdebug("Status: %s, Reason: %s\n", status.show().toUtf8().constData(), status.status().toUtf8().constData());

	XMPP::Status newStatus = status;

	// TODO: Check if Caps is enabled
	// Send entity capabilities
	newStatus.setCapsNode(capsNode());
	newStatus.setCapsVersion(capsVersion());
	newStatus.setCapsHashAlgorithm(QLatin1String("sha-1"));
	newStatus.setCapsExt(capsExt());
	newStatus.setStatus(status.status());

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
			kdebug("Sending new presence to the server.\n");

			client()->setPresence(newStatus);
		}
		else
		{
			kdebug("We were not connected, presence update aborted.\n");
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
	XMPP::JT_Presence *task = new XMPP::JT_Presence(Client->rootTask());
	task->sub(jid, type);
	task->go(true);
}

void JabberClient::slotResourceAvailable(const XMPP::Jid &jid, const Resource &resource)
{
	emit resourceAvailable(jid, resource);
}

void JabberClient::slotResourceUnavailable(const XMPP::Jid &jid, const Resource &resource)
{
	emit resourceUnavailable(jid, resource);
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

void JabberClient::serverFeaturesChanged()
{
	if (serverInfoManager)
		setPEPAvailable(serverInfoManager->hasPEP());
}

void JabberClient::setPEPAvailable(bool b)
{
	if (PepAvailable == b)
		return;

	PepAvailable = b;

	// Publish support
	if (b && client()->extensions().contains("ep"))
	{
		QStringList pepNodes;
		/*pepNodes += "http://jabber.org/protocol/mood";
		pepNodes += "http://jabber.org/protocol/tune";
		pepNodes += "http://jabber.org/protocol/physloc";
		pepNodes += "http://jabber.org/protocol/geoloc";*/
		pepNodes += "http://www.xmpp.org/extensions/xep-0084.html#ns-data";
		pepNodes += "http://www.xmpp.org/extensions/xep-0084.html#ns-metadata";
		client()->addExtension("ep", XMPP::Features(pepNodes));
		//setStatusActual(d->loginStatus);
	}
	else if (!b && client()->extensions().contains("ep"))
		client()->removeExtension("ep");
}

QString JabberClient::calculateCapsVersion(const DiscoItem::Identity &identity, const QStringList &features)
{
	QString result(identity.category);
	result.append('/');
	result.append(identity.type);
	result.append("//");
	result.append(identity.name);
	result.append('<');
	result.append(features.join(QLatin1String("<")));
	result.append('<');

	return QString::fromAscii(QCryptographicHash::hash(result.toAscii(), QCryptographicHash::Sha1).toBase64());
}

}
