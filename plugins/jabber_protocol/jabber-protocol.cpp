/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QCoreApplication>
#include <QtCrypto/QtCrypto>
#include <QNetworkProxy>

#include <qxmpp/QXmppClient.h>

#include "buddies/buddy-manager.h"
#include "buddies/group-manager.h"
#include "chat/chat-manager.h"
#include "contacts/contact-manager.h"
#include "core/core.h"
#include "gui/windows/message-dialog.h"
#include "os/generic/system-info.h"
#include "protocols/protocols-manager.h"
#include "status/status-type-manager.h"
#include "url-handlers/url-handler-manager.h"
#include "debug.h"

#include "actions/jabber-actions.h"
#include "actions/jabber-protocol-menu-manager.h"
#include "certificates/trusted-certificates-manager.h"
#include "resource/jabber-resource-pool.h"
#include "services/jabber-chat-service.h"
#include "services/jabber-chat-state-service.h"
#include "services/jabber-client-info-service.h"
#include "services/jabber-pep-service.h"
#include "services/jabber-room-chat-service.h"
#include "services/jabber-roster-service.h"
#include "services/jabber-server-info-service.h"
#include "services/jabber-stream-debug-service.h"
#include "services/jabber-subscription-service.h"
#include "services/jabber-vcard-service.h"
#include "facebook-protocol-factory.h"
#include "gtalk-protocol-factory.h"
#include "iris-status-adapter.h"
#include "jabber-contact-details.h"
#include "jabber-id-validator.h"
#include "jabber-protocol-factory.h"
#include "jabber-url-handler.h"

#include "jabber-protocol.h"

JabberProtocol::JabberProtocol(Account account, ProtocolFactory *factory) :
		Protocol(account, factory), ResourcePool(0),
		ContactsListReadOnly(false)
{
	kdebugf();

	if (account.id().endsWith(QLatin1String("@chat.facebook.com")))
		setContactsListReadOnly(true);

	m_client = new QXmppClient{this};
	connect(m_client, SIGNAL(connected()), this, SLOT(connectedToServer()));
	connect(m_client, SIGNAL(disconnected()), this, SLOT(disconenctedFromServer()));
	connect(m_client, SIGNAL(error(QXmppClient::Error)), this, SLOT(error(QXmppClient::Error)));
	connect(m_client, SIGNAL(presenceReceived(QXmppPresence)), this, SLOT(presenceReceived(QXmppPresence)));
/*
	XmppClient = new Client(this);
	connect(XmppClient, SIGNAL(disconnected()), this, SLOT(connectionError()));
	connect(XmppClient, SIGNAL(resourceAvailable(Jid,Resource)), this, SLOT(clientAvailableResourceReceived(Jid,Resource)));
	connect(XmppClient, SIGNAL(resourceUnavailable(Jid,Resource)), this, SLOT(clientUnavailableResourceReceived(Jid,Resource)));
*/
	auto roomChatService = new JabberRoomChatService{account, this};
	roomChatService->setBuddyManager(BuddyManager::instance());
	roomChatService->setChatManager(ChatManager::instance());
	roomChatService->setContactManager(ContactManager::instance());
	//roomChatService->setXmppClient(XmppClient);

	CurrentAvatarService = new JabberAvatarService(account, this);
	JabberChatService *chatService = new JabberChatService(account, this);
	chatService->setFormattedStringFactory(Core::instance()->formattedStringFactory());
	chatService->setRawMessageTransformerService(Core::instance()->rawMessageTransformerService());
	chatService->setRoomChatService(roomChatService);

	JabberChatStateService *chatStateService = new JabberChatStateService(account, this);
	CurrentContactPersonalInfoService = new JabberContactPersonalInfoService(account, this);
	CurrentFileTransferService = new JabberFileTransferService(this);
	CurrentPersonalInfoService = new JabberPersonalInfoService(account, this);
	CurrentClientInfoService = new JabberClientInfoService(this);

	CurrentServerInfoService = new JabberServerInfoService(this);
	connect(CurrentServerInfoService, SIGNAL(updated()), this, SLOT(serverInfoUpdated()));

	CurrentPepService = new JabberPepService(this);

	CurrentAvatarService->setPepService(CurrentPepService);

	CurrentStreamDebugService = new JabberStreamDebugService(this);

	CurrentVCardService = new JabberVCardService(account, this);
	// CurrentVCardService->setXmppClient(XmppClient);

	CurrentAvatarService->setVCardService(CurrentVCardService);
	CurrentContactPersonalInfoService->setVCardService(CurrentVCardService);
	CurrentPersonalInfoService->setVCardService(CurrentVCardService);

	QStringList features;
	features
			<< "http://jabber.org/protocol/chatstates"
			<< "jabber:iq:version"
			<< "jabber:x:data"
			<< "urn:xmpp:avatar:data"
			<< "urn:xmpp:avatar:metadata"
			<< "urn:xmpp:avatar:metadata+notify";

	CurrentClientInfoService->setFeatures(features);

	// connect(XmppClient, SIGNAL(messageReceived(const Message &)),
	//         chatService, SLOT(handleReceivedMessage(Message)));
	// connect(XmppClient, SIGNAL(messageReceived(const Message &)),
	//         chatStateService, SLOT(handleReceivedMessage(const Message &)));
	connect(chatService, SIGNAL(messageAboutToSend(Message&)),
	        chatStateService, SLOT(handleMessageAboutToSend(Message&)));

	auto contacts = ContactManager::instance()->contacts(account, ContactManager::ExcludeAnonymous);
	JabberRosterService *rosterService = new JabberRosterService(this, contacts, this);

	// chatService->setXmppClient(XmppClient);
	// chatStateService->setClient(XmppClient);
	// rosterService->setClient(XmppClient);

	connect(rosterService, SIGNAL(rosterReady(bool)),
			this, SLOT(rosterReady(bool)));

	setChatService(chatService);
	setRosterService(rosterService);

	CurrentSubscriptionService = new JabberSubscriptionService(this);

	kdebugf2();
}

JabberProtocol::~JabberProtocol()
{
	logout();
}

void JabberProtocol::setContactsListReadOnly(bool contactsListReadOnly)
{
	ContactsListReadOnly = contactsListReadOnly;
}

void JabberProtocol::serverInfoUpdated()
{
	CurrentPepService->setEnabled(CurrentServerInfoService->supportsPep());
}

void JabberProtocol::rosterReady(bool success)
{
	Q_UNUSED(success)

	/* Since we are online now, set initial presence. Don't do this
	* before the roster request or we will receive presence
	* information before we have updated our roster with actual
	* contacts from the server! (Iris won't forward presence
	* information in that case either). */
	kdebug("Setting initial presence...\n");

	sendStatusToServer();
	CurrentServerInfoService->requestServerInfo();
}

/*
 * login procedure
 *
 * After calling login method we set up JabberClient that must call connectedToServer in order to inform
 * us that connection was established. Then we can tell this to state machine in Protocol class
 */
void JabberProtocol::login()
{
	kdebugf();

	JabberAccountDetails *jabberAccountDetails = dynamic_cast<JabberAccountDetails *>(account().details());
	if (!jabberAccountDetails)
	{
		connectionClosed();
		return;
	}
/*
	if (jabberAccountDetails->publishSystemInfo())
	{
		CurrentClientInfoService->setClientName("Kadu");
		CurrentClientInfoService->setClientVersion(Core::instance()->version());
		CurrentClientInfoService->setOSName(SystemInfo::instance()->osFullName());
	}
	else
	{
		CurrentClientInfoService->setClientName(QString());
		CurrentClientInfoService->setClientVersion(QString());
		CurrentClientInfoService->setOSName(QString());
	}

	CurrentConnectionService->connectToServer();
*/

	auto details = dynamic_cast<JabberAccountDetails *>(account().details());
	if (!details)
		return;

	auto streamSecurityMode = JabberAccountDetails::Encryption_No == details->encryptionMode()
			? QXmppConfiguration::StreamSecurityMode::TLSDisabled
			: JabberAccountDetails::Encryption_Yes == details->encryptionMode()
			? QXmppConfiguration::StreamSecurityMode::TLSRequired
			: QXmppConfiguration::StreamSecurityMode::TLSEnabled;
	auto useNonSASLAuthentication = details->plainAuthMode() == JabberAccountDetails::AllowPlain
			? true
			: details->plainAuthMode() == JabberAccountDetails::JabberAccountDetails::AllowPlainOverTLS
			? QXmppConfiguration::StreamSecurityMode::TLSDisabled != streamSecurityMode
			: false;

	if (streamSecurityMode != QXmppConfiguration::StreamSecurityMode::TLSDisabled && !QCA::isSupported("tls"))
	{
		// emit some error
		// emit connectionClosed(tr("SSL support could not be initialized for account %1. This is most likely because the QCA TLS plugin is not installed on your system."));
		return;
	}

	auto configuration = QXmppConfiguration{};
	configuration.setAutoAcceptSubscriptions(false);
	configuration.setAutoReconnectionEnabled(true);
	configuration.setIgnoreSslErrors(false);
	configuration.setJid(account().id());
	configuration.setPassword(account().password());
	configuration.setResource(details->resource());
	configuration.setStreamSecurityMode(streamSecurityMode);
	configuration.setUseNonSASLAuthentication(useNonSASLAuthentication);

	if (account().proxy())
	{
		auto proxy = QNetworkProxy{};
		if (account().proxy().type() == "socks")
			proxy.setType(QNetworkProxy::Socks5Proxy);
		else
			proxy.setType(QNetworkProxy::HttpProxy);

		proxy.setHostName(account().proxy().address());
		proxy.setPort(account().proxy().port());
		proxy.setUser(account().proxy().user());
		proxy.setPassword(account().proxy().password());
		configuration.setNetworkProxy(proxy);
	}

	if (details->useCustomHostPort())
	{
		auto atIndex = account().id().indexOf("@");
		if (atIndex >= 0)
			configuration.setDomain(account().id().mid(atIndex + 1));
		configuration.setHost(details->customHost());
		configuration.setPort(details->customPort());
	}

	// m_client->logger()->setLoggingType(QXmppLogger::StdoutLogging);
	m_client->connectToServer(configuration);


/*


	if (!XmppClient)
		return;

	cleanUp();

	MyJid = Jid(ParentProtocol->account().id()).withResource(details->resource());
	Password = ParentProtocol->account().password();

	Connector = createConnector();

	if (forceTLS() || useSSL())
	{
		TLSHandler = createTLSHandler();
		connect(TLSHandler.data(), SIGNAL(tlsHandshaken()), SLOT(tlsHandshaken()));

		QString host = details->useCustomHostPort() ? details->customHost() : Jid(ParentProtocol->account().id()).domain();
		TLSHandler->startClient(host);
	}

	Stream = createClientStream(Connector.data(), TLSHandler.data());
	connect(Stream.data(), SIGNAL(needAuthParams(bool, bool, bool)), this, SLOT(streamNeedAuthParams(bool, bool, bool)));
	connect(Stream.data(), SIGNAL(authenticated()), this, SLOT(streamAuthenticated()));
	connect(Stream.data(), SIGNAL(connectionClosed()), this, SIGNAL(connectionError()));
	connect(Stream.data(), SIGNAL(delayedCloseFinished()), this, SIGNAL(connectionError()));
	connect(Stream.data(), SIGNAL(warning(int)), this, SLOT(streamWarning(int)));
	connect(Stream.data(), SIGNAL(error(int)), this, SLOT(streamError(int)));

	XmppClient->connectToServer(Stream.data(), MyJid, true);*/


	kdebugf2();
}

/*
 * We are now connected to server - login procedure has ended
 */
void JabberProtocol::connectedToServer()
{
	loggedIn();
}

void JabberProtocol::afterLoggedIn()
{
	static_cast<JabberRosterService *>(rosterService())->prepareRoster();
}

void JabberProtocol::logout()
{
	m_client->disconnectFromServer();

	loggedOut();
}

void JabberProtocol::disconenctedFromServer()
{
}

void JabberProtocol::error(QXmppClient::Error error)
{
	switch (error)
	{
		case QXmppClient::Error::SocketError:
			emit connectionError(account(), m_client->configuration().host(), QString{}); // TODO: add message
			connectionError();
			break;
		case QXmppClient::Error::KeepAliveError:
			emit connectionError(account(), m_client->configuration().host(), QString{}); // TODO: add message
			connectionError();
			break;
		case QXmppClient::Error::XmppStreamError:
			switch (m_client->xmppStreamError())
			{
				case QXmppStanza::Error::NotAuthorized:
				case QXmppStanza::Error::BadAuth:
					passwordRequired();
					break;
				default:
					emit connectionError(account(), m_client->configuration().host(), QString{}); // TODO: add message
					connectionError();
					break;
			}
			break;
		default:
			break;
	}
}

void JabberProtocol::sendStatusToServer()
{
	if (!isConnected() && !isDisconnecting())
		return;

	auto presence = QXmppPresence{};
	presence.setType(QXmppPresence::Available);

	switch (status().type())
	{
		case StatusTypeFreeForChat:
			presence.setAvailableStatusType(QXmppPresence::Chat);
			break;
		case StatusTypeOnline:
			presence.setAvailableStatusType(QXmppPresence::Online);
			break;
		case StatusTypeAway:
			presence.setAvailableStatusType(QXmppPresence::Away);
			break;
		case StatusTypeNotAvailable:
			presence.setAvailableStatusType(QXmppPresence::XA);
			break;
		case StatusTypeDoNotDisturb:
			presence.setAvailableStatusType(QXmppPresence::DND);
			break;
		case StatusTypeInvisible:
			presence.setAvailableStatusType(QXmppPresence::DND);
			break;
		case StatusTypeOffline:
		default:
			presence.setType(QXmppPresence::Unavailable);
			break;
	}

//	CurrentClientInfoService->fillStatusCapsData(xmppStatus); TODO fix

	auto details = dynamic_cast<JabberAccountDetails *>(account().details());
	if (details)
	{
		presence.setPriority(details->priority());
		// presence.setResource(); not available!
		// resourcePool()->addResource(CurrentConnectionService->jid(), newResource);
		// resourcePool()->lockToResource(CurrentConnectionService->jid(), newResource);
	}

	m_client->setClientPresence(presence);

	account().accountContact().setCurrentStatus(status());
}

void JabberProtocol::changePrivateMode()
{
	sendStatusToServer();
}
/*
void JabberProtocol::clientAvailableResourceReceived(const Jid &jid, const Resource &resource)
{
	kdebug("New resource available for %s\n", jid.full().toUtf8().constData());

	resourcePool()->addResource(jid, resource);

	Resource bestResource = resourcePool()->bestResource(jid);

	if (resource.name() == bestResource.name())
		notifyAboutPresenceChanged(jid, resource);

	kdebugf2();
}

void JabberProtocol::clientUnavailableResourceReceived(const Jid &jid, const Resource &resource)
{
	kdebug("New resource unavailable for %s\n", jid.full().toUtf8().constData());

	Resource bestResource = resourcePool()->bestResource(jid);

	bool notify = bestResource.name() == resource.name();

	resourcePool()->removeResource(jid, resource);

	bestResource = resourcePool()->bestResource(jid);

	if (notify)
		notifyAboutPresenceChanged(jid, bestResource.name() == JabberResourcePool::EmptyResource.name()
									? resource : bestResource);
}

void JabberProtocol::notifyAboutPresenceChanged(const Jid &jid, const Resource &resource)
{
	::Status status(IrisStatusAdapter::fromIrisStatus(resource.status()));
	Contact contact = ContactManager::instance()->byId(account(), jid.bare(), ActionReturnNull);

	if (!contact)
		return;

	::Status oldStatus = contact.currentStatus();
	contact.setCurrentStatus(status);

	// see issue #2159 - we need a way to ignore first status of given contact
	if (contact.ignoreNextStatusChange())
		contact.setIgnoreNextStatusChange(false);
	else
		emit contactStatusChanged(contact, oldStatus);
}
*/

void JabberProtocol::presenceReceived(const QXmppPresence &presence)
{
	if (presence.isMucSupported())
		return;

	auto id = presence.from();
	auto resourceIndex = id.indexOf('/');
	if (resourceIndex >= 0)
		id = id.mid(0, resourceIndex);

	auto contact = ContactManager::instance()->byId(account(), id, ActionReturnNull);
	if (!contact)
		return;

	auto status = Status{};
	if (presence.type() == QXmppPresence::Available)
	{
		switch (presence.availableStatusType())
		{
			case QXmppPresence::AvailableStatusType::Online:
				status.setType(StatusTypeOnline);
				break;
			case QXmppPresence::AvailableStatusType::Away:
				status.setType(StatusTypeAway);
				break;
			case QXmppPresence::AvailableStatusType::XA:
				status.setType(StatusTypeNotAvailable);
				break;
			case QXmppPresence::AvailableStatusType::DND:
				status.setType(StatusTypeDoNotDisturb);
				break;
			case QXmppPresence::AvailableStatusType::Chat:
				status.setType(StatusTypeFreeForChat);
				break;
			case QXmppPresence::AvailableStatusType::Invisible:
				status.setType(StatusTypeDoNotDisturb);
				break;
		}
	}
	else if (presence.type() == QXmppPresence::Unavailable)
		status.setType(StatusTypeOffline);
	else
		return;

	status.setDescription(presence.statusText());

	auto oldStatus = contact.currentStatus();
	contact.setCurrentStatus(status);

	// see issue #2159 - we need a way to ignore first status of given contact
	if (contact.ignoreNextStatusChange())
		contact.setIgnoreNextStatusChange(false);
	else
		emit contactStatusChanged(contact, oldStatus);
}

JabberResourcePool *JabberProtocol::resourcePool()
{
	if (!ResourcePool)
		ResourcePool = new JabberResourcePool(this);

	return ResourcePool;
}

QString JabberProtocol::statusPixmapPath()
{
	return QLatin1String("xmpp");
}

JabberContactDetails * JabberProtocol::jabberContactDetails(Contact contact) const
{
	if (contact.isNull())
		return 0;

	return dynamic_cast<JabberContactDetails *>(contact.details());
}

#include "moc_jabber-protocol.cpp"
