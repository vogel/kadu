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

#include "jabber-protocol.h"

#include "actions/jabber-actions.h"
#include "actions/jabber-protocol-menu-manager.h"
#include "qxmpp/jabber-register-extension.h"
#include "qxmpp/jabber-roster-extension.h"
#include "qxmpp/jabber-ssl-handler.h"
#include "services/jabber-change-password-service.h"
#include "services/jabber-chat-service.h"
#include "services/jabber-chat-state-service.h"
#include "services/jabber-error-service.h"
#include "services/jabber-file-transfer-service.h"
#include "services/jabber-presence-service.h"
#include "services/jabber-register-account-service.h"
#include "services/jabber-resource-service.h"
#include "services/jabber-room-chat-service.h"
#include "services/jabber-roster-service.h"
#include "services/jabber-stream-debug-service.h"
#include "services/jabber-subscription-service.h"
#include "services/jabber-vcard-service.h"
#include "gtalk-protocol-factory.h"
#include "jabber-id-validator.h"
#include "jabber-protocol-factory.h"
#include "jabber-url-handler.h"
#include "jid.h"

#include "avatars/avatar-manager.h"
#include "buddies/buddy-manager.h"
#include "buddies/group-manager.h"
#include "chat/chat-manager.h"
#include "contacts/contact-manager.h"
#include "core/core.h"
#include "gui/windows/message-dialog.h"
#include "misc/memory.h"
#include "os/generic/system-info.h"
#include "protocols/protocols-manager.h"
#include "status/status-type-manager.h"
#include "url-handlers/url-handler-manager.h"

#include <QtCore/QCoreApplication>
#include <QtNetwork/QNetworkProxy>
#include <QtNetwork/QSslSocket>
#include <qxmpp/QXmppClient.h>
#include <qxmpp/QXmppMucManager.h>
#include <qxmpp/QXmppRosterManager.h>
#include <qxmpp/QXmppTransferManager.h>
#include <qxmpp/QXmppVersionManager.h>
#include <qxmpp/QXmppVCardManager.h>

JabberProtocol::JabberProtocol(Account account, ProtocolFactory *factory) :
		Protocol{account, factory},
		m_contactsListReadOnly(false)
{
	auto details = dynamic_cast<JabberAccountDetails *>(account.details());
	connect(details, SIGNAL(priorityChanged()), this, SLOT(updatePresence()), Qt::UniqueConnection);

	// TODO: remove after 01.05.2015
	if (account.id().endsWith(QLatin1String("@chat.facebook.com")))
		setContactsListReadOnly(true);

	m_presenceService = new JabberPresenceService{this};
	m_errorService = new JabberErrorService{this};

	m_client = new QXmppClient{this};
	connect(m_client, SIGNAL(connected()), this, SLOT(connectedToServer()));
	connect(m_client, SIGNAL(disconnected()), this, SLOT(disconenctedFromServer()));
	connect(m_client, SIGNAL(error(QXmppClient::Error)), this, SLOT(error(QXmppClient::Error)));
	connect(m_client, SIGNAL(presenceReceived(QXmppPresence)), this, SLOT(presenceReceived(QXmppPresence)));

	new JabberSslHandler{m_client};

	m_registerExtension = make_unique<JabberRegisterExtension>();
	m_rosterExtension = make_unique<JabberRosterExtension>();
	m_mucManager = make_unique<QXmppMucManager>();
	m_transferManager = make_unique<QXmppTransferManager>();

	m_rosterExtension->setJabberErrorService(m_errorService);

	m_client->addExtension(m_registerExtension.get());
	m_client->insertExtension(0, m_rosterExtension.get());
	m_client->addExtension(m_mucManager.get());
	m_client->addExtension(m_transferManager.get());

	m_changePasswordService = new JabberChangePasswordService{m_registerExtension.get(), this};
	m_changePasswordService->setErrorService(m_errorService);

	m_resourceService = new JabberResourceService{this};

	m_roomChatService = new JabberRoomChatService{m_client, m_mucManager.get(), account, this};
	m_roomChatService->setBuddyManager(BuddyManager::instance());
	m_roomChatService->setChatManager(ChatManager::instance());
	m_roomChatService->setContactManager(ContactManager::instance());
	m_roomChatService->initialize();

	auto chatStateService = new JabberChatStateService{m_client, account, this};
	chatStateService->setResourceService(m_resourceService);

	m_avatarService = new JabberAvatarService{m_client, account, this};
	m_avatarService->setAvatarManager(AvatarManager::instance());
	m_avatarService->setContactManager(ContactManager::instance());

	auto chatService = new JabberChatService{m_client, account, this};
	chatService->setFormattedStringFactory(Core::instance()->formattedStringFactory());
	chatService->setRawMessageTransformerService(Core::instance()->rawMessageTransformerService());
	chatService->setChatStateService(chatStateService);
	chatService->setResourceService(m_resourceService);
	chatService->setRoomChatService(m_roomChatService);

	m_contactPersonalInfoService = new JabberContactPersonalInfoService{account, this};
	m_personalInfoService = new JabberPersonalInfoService{account, this};
	m_streamDebugService = new JabberStreamDebugService{m_client, this};

	m_fileTransferService = new JabberFileTransferService{m_transferManager.get(), account, this};
	m_fileTransferService->setResourceService(m_resourceService);

	m_vcardService = new JabberVCardService{&m_client->vCardManager(), this};

	m_avatarService->setVCardService(m_vcardService);
	m_contactPersonalInfoService->setVCardService(m_vcardService);
	m_personalInfoService->setVCardService(m_vcardService);

	auto contacts = ContactManager::instance()->contacts(account, ContactManager::ExcludeAnonymous);
	auto rosterService = new JabberRosterService{&m_client->rosterManager(), m_rosterExtension.get(), contacts, this};

	connect(rosterService, SIGNAL(rosterReady()), this, SLOT(rosterReady()));

	setChatService(chatService);
	setChatStateService(chatStateService);
	setRosterService(rosterService);

	m_subscriptionService = new JabberSubscriptionService{&m_client->rosterManager(), this};
	m_subscriptionService->setContactManager(ContactManager::instance());
}

JabberProtocol::~JabberProtocol()
{
	logout();
}

void JabberProtocol::setContactsListReadOnly(bool contactsListReadOnly)
{
	m_contactsListReadOnly = contactsListReadOnly;
}

void JabberProtocol::rosterReady()
{
	/* Since we are online now, set initial presence. Don't do this
	* before the roster request or we will receive presence
	* information before we have updated our roster with actual
	* contacts from the server! (Iris won't forward presence
	* information in that case either). */
	sendStatusToServer();
}

/*
 * login procedure
 *
 * After calling login method we set up JabberClient that must call connectedToServer in order to inform
 * us that connection was established. Then we can tell this to state machine in Protocol class
 */
void JabberProtocol::login()
{
	auto jabberAccountDetails = dynamic_cast<JabberAccountDetails *>(account().details());
	if (!jabberAccountDetails)
	{
		connectionClosed();
		return;
	}

	if (jabberAccountDetails->publishSystemInfo())
	{
		m_client->versionManager().setClientName("Kadu");
		m_client->versionManager().setClientVersion(Core::instance()->version());
		m_client->versionManager().setClientOs(SystemInfo::instance()->osFullName());
	}
	else
	{
		m_client->versionManager().setClientName(QString{});
		m_client->versionManager().setClientVersion(QString{});
		m_client->versionManager().setClientOs(QString{});
	}

	auto details = dynamic_cast<JabberAccountDetails *>(account().details());
	if (!details)
		return;

	auto streamSecurityMode = QXmppConfiguration::StreamSecurityMode{};
	switch (details->encryptionMode())
	{
		case JabberAccountDetails::Encryption_Auto:
			streamSecurityMode = QXmppConfiguration::StreamSecurityMode::TLSEnabled;
			break;
		case JabberAccountDetails::Encryption_Yes:
			streamSecurityMode = QXmppConfiguration::StreamSecurityMode::TLSRequired;
			break;
		case JabberAccountDetails::Encryption_No:
			streamSecurityMode = QXmppConfiguration::StreamSecurityMode::TLSDisabled;
			break;
		case JabberAccountDetails::Encryption_Legacy:
			streamSecurityMode = QXmppConfiguration::StreamSecurityMode::LegacySSL;
			break;
	}

	auto useNonSASLAuthentication = details->plainAuthMode() == JabberAccountDetails::AllowPlain
			? true
			: details->plainAuthMode() == JabberAccountDetails::JabberAccountDetails::AllowPlainOverTLS
			? QXmppConfiguration::StreamSecurityMode::TLSDisabled != streamSecurityMode
			: false;

	auto jid = Jid::parse(account().id()).withResource(details->resource());

	auto configuration = QXmppConfiguration{};
	configuration.setAutoAcceptSubscriptions(false);
	configuration.setAutoReconnectionEnabled(false);
	configuration.setIgnoreSslErrors(false);
	configuration.setJid(jid.full());
	configuration.setPassword(account().password());
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
		configuration.setHost(details->customHost());
		configuration.setPort(details->customPort());
	}

	auto presence = m_presenceService->statusToPresence(status());
	if (details)
		presence.setPriority(details->priority());

	static_cast<JabberRosterService *>(rosterService())->prepareRoster();
	m_client->connectToServer(configuration, presence);
}

void JabberProtocol::connectedToServer()
{
	loggedIn();
}

void JabberProtocol::logout()
{
	auto logoutStatus = status();
	logoutStatus.setType(StatusTypeOffline);
	m_client->setClientPresence(m_presenceService->statusToPresence(logoutStatus));
	m_client->disconnectFromServer();

	loggedOut();
}

void JabberProtocol::disconenctedFromServer()
{
	m_resourceService->clear();
}

void JabberProtocol::error(QXmppClient::Error error)
{
	auto errorMessage = QString{};
	if (error == QXmppClient::Error::XmppStreamError)
	{
		switch (m_client->xmppStreamError())
		{
			case QXmppStanza::Error::NotAuthorized:
				passwordRequired();
				return;
			case QXmppStanza::Error::Conflict:
				errorMessage = tr("Another client connected on the same resource.");
				setStatus({}, SourceUser);
				break;
			default:
				break;
		}
	}

	if (errorMessage.isEmpty())
		errorMessage = m_errorService->errorMessage(m_client, error);
	emit connectionError(account(), m_client->configuration().host(), errorMessage);
	connectionError();

	m_client->disconnectFromServer();
}

void JabberProtocol::updatePresence()
{
	sendStatusToServer();
}

void JabberProtocol::sendStatusToServer()
{
	if (!isConnected() && !isDisconnecting())
		return;

	auto presence = m_presenceService->statusToPresence(status());
	auto details = dynamic_cast<JabberAccountDetails *>(account().details());
	if (details)
		presence.setPriority(details->priority());

	m_client->setClientPresence(presence);
	account().accountContact().setCurrentStatus(status());
}

void JabberProtocol::changePrivateMode()
{
	sendStatusToServer();
}

void JabberProtocol::presenceReceived(const QXmppPresence &presence)
{
	if (presence.isMucSupported())
		return;

	auto jid = Jid::parse(presence.from());
	auto id = jid.bare();
	auto contact = ContactManager::instance()->byId(account(), id, ActionReturnNull);
	if (!contact)
		return;

	auto status = m_presenceService->presenceToStatus(presence);
	if (status.type() != StatusTypeOffline)
	{
		auto jabberResource = JabberResource{jid, presence.priority(), status};
		m_resourceService->updateResource(jabberResource);
	}
	else
	{
		if (contact.property("jabber:chat-resource", QString{}).toString() == jid.resource())
			contact.removeProperty("jabber:chat-resource");
		m_resourceService->removeResource(jid);
	}

	auto bestResource = m_resourceService->bestResource(id);
	auto statusToSet = bestResource.isEmpty()
			? status
			: bestResource.status();

	auto oldStatus = contact.currentStatus();
	contact.setCurrentStatus(statusToSet);

	// see issue #2159 - we need a way to ignore first status of given contact
	if (contact.ignoreNextStatusChange())
		contact.setIgnoreNextStatusChange(false);
	else
		emit contactStatusChanged(contact, oldStatus);
}

QString JabberProtocol::statusPixmapPath()
{
	return QLatin1String("xmpp");
}

JabberChangePasswordService * JabberProtocol::changePasswordService() const
{
	return m_changePasswordService;
}

#include "moc_jabber-protocol.cpp"
