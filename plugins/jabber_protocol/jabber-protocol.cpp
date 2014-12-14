/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009, 2010, 2010, 2010, 2011, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Piotr Pełzowski (floss@pelzowski.eu)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2009, 2010 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2009, 2010, 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include <QtCrypto>

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
#include "iris/filetransfer.h"
#include "iris/irisnetglobal.h"
#include "resource/jabber-resource-pool.h"
#include "services/jabber-chat-service.h"
#include "services/jabber-chat-state-service.h"
#include "services/jabber-client-info-service.h"
#include "services/jabber-connection-service.h"
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

namespace XMPP
{

JabberProtocol::JabberProtocol(Account account, ProtocolFactory *factory) :
		Protocol(account, factory), ResourcePool(0),
		ContactsListReadOnly(false)
{
	kdebugf();

	if (account.id().endsWith(QLatin1String("@chat.facebook.com")))
		setContactsListReadOnly(true);

	XmppClient = new XMPP::Client(this);
	connect(XmppClient, SIGNAL(disconnected()), this, SLOT(connectionError()));
	connect(XmppClient, SIGNAL(resourceAvailable(Jid,Resource)), this, SLOT(clientAvailableResourceReceived(Jid,Resource)));
	connect(XmppClient, SIGNAL(resourceUnavailable(Jid,Resource)), this, SLOT(clientUnavailableResourceReceived(Jid,Resource)));

	auto roomChatService = new XMPP::JabberRoomChatService{account, this};
	roomChatService->setBuddyManager(BuddyManager::instance());
	roomChatService->setChatManager(ChatManager::instance());
	roomChatService->setContactManager(ContactManager::instance());
	roomChatService->setXmppClient(XmppClient);

	CurrentAvatarService = new JabberAvatarService(account, this);
	XMPP::JabberChatService *chatService = new XMPP::JabberChatService(account, this);
	chatService->setFormattedStringFactory(Core::instance()->formattedStringFactory());
	chatService->setRawMessageTransformerService(Core::instance()->rawMessageTransformerService());
	chatService->setRoomChatService(roomChatService);

	XMPP::JabberChatStateService *chatStateService = new XMPP::JabberChatStateService(account, this);
	CurrentContactPersonalInfoService = new JabberContactPersonalInfoService(account, this);
	CurrentFileTransferService = new JabberFileTransferService(this);
	CurrentPersonalInfoService = new JabberPersonalInfoService(account, this);
	CurrentClientInfoService = new XMPP::JabberClientInfoService(this);

	CurrentServerInfoService = new XMPP::JabberServerInfoService(this);
	connect(CurrentServerInfoService, SIGNAL(updated()), this, SLOT(serverInfoUpdated()));

	CurrentPepService = new JabberPepService(this);

	CurrentAvatarService->setPepService(CurrentPepService);

	CurrentConnectionService = new XMPP::JabberConnectionService(this);
	connect(CurrentConnectionService, SIGNAL(connected()), this, SLOT(connectedToServer()));
	connect(CurrentConnectionService, SIGNAL(connectionClosed(QString)), this, SLOT(connectionClosedSlot(QString)));
	connect(CurrentConnectionService, SIGNAL(connectionError(QString)), this, SLOT(connectionErrorSlot(QString)));
	connect(CurrentConnectionService, SIGNAL(invalidPassword()), this, SLOT(passwordRequired()));
	connect(CurrentConnectionService, SIGNAL(tlsCertificateAccepted()), this, SLOT(reconnect()));

	CurrentStreamDebugService = new XMPP::JabberStreamDebugService(this);

	CurrentVCardService = new XMPP::JabberVCardService(account, this);
	CurrentVCardService->setXmppClient(XmppClient);

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

	connect(XmppClient, SIGNAL(messageReceived(const Message &)),
	        chatService, SLOT(handleReceivedMessage(Message)));
	connect(XmppClient, SIGNAL(messageReceived(const Message &)),
	        chatStateService, SLOT(handleReceivedMessage(const Message &)));
	connect(chatService, SIGNAL(messageAboutToSend(Message&)),
	        chatStateService, SLOT(handleMessageAboutToSend(Message&)));

	auto contacts = ContactManager::instance()->contacts(account, ContactManager::ExcludeAnonymous);
	XMPP::JabberRosterService *rosterService = new XMPP::JabberRosterService(this, contacts, this);

	chatService->setXmppClient(XmppClient);
	chatStateService->setClient(XmppClient);
	rosterService->setClient(XmppClient);

	connect(rosterService, SIGNAL(rosterReady(bool)),
			this, SLOT(rosterReady(bool)));

	setChatService(chatService);
	setRosterService(rosterService);

	CurrentSubscriptionService = new XMPP::JabberSubscriptionService(this);

	kdebugf2();
}

JabberProtocol::~JabberProtocol()
{
	logout();
}

XMPP::Client * JabberProtocol::xmppClient()
{
	return XmppClient;
}

void JabberProtocol::setContactsListReadOnly(bool contactsListReadOnly)
{
	ContactsListReadOnly = contactsListReadOnly;
}

void JabberProtocol::connectionClosedSlot(const QString &message)
{
	emit connectionError(account(), CurrentConnectionService->host(), message);
	connectionClosed();
}

void JabberProtocol::connectionErrorSlot(const QString& message)
{
	emit connectionError(account(), CurrentConnectionService->host(), message);
	connectionError();
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
	CurrentConnectionService->disconnectFromServer(status());

	loggedOut();
}

void JabberProtocol::sendStatusToServer()
{
	if (!isConnected() && !isDisconnecting())
		return;

	XMPP::Status xmppStatus = IrisStatusAdapter::toIrisStatus(status());
	CurrentClientInfoService->fillStatusCapsData(xmppStatus);

	JabberAccountDetails *jabberAccountDetails = dynamic_cast<JabberAccountDetails *>(account().details());
	if (jabberAccountDetails)
	{
		xmppStatus.setPriority(jabberAccountDetails->priority());

		XMPP::Resource newResource(jabberAccountDetails->resource(), xmppStatus);

		// update our resource in the resource pool
		resourcePool()->addResource(CurrentConnectionService->jid(), newResource);

		// make sure that we only consider our own resource locally
		resourcePool()->lockToResource(CurrentConnectionService->jid(), newResource);
	}

	if (xmppClient()->isActive() && xmppStatus.show() != QString("connecting"))
		xmppClient()->setPresence(xmppStatus);

	account().accountContact().setCurrentStatus(status());
}

void JabberProtocol::changePrivateMode()
{
	//sendStatusToServer();
}

void JabberProtocol::clientAvailableResourceReceived(const XMPP::Jid &jid, const XMPP::Resource &resource)
{
	kdebug("New resource available for %s\n", jid.full().toUtf8().constData());

	resourcePool()->addResource(jid, resource);

	XMPP::Resource bestResource = resourcePool()->bestResource(jid);

	if (resource.name() == bestResource.name())
		notifyAboutPresenceChanged(jid, resource);

	kdebugf2();
}

void JabberProtocol::clientUnavailableResourceReceived(const XMPP::Jid &jid, const XMPP::Resource &resource)
{
	kdebug("New resource unavailable for %s\n", jid.full().toUtf8().constData());

	XMPP::Resource bestResource = resourcePool()->bestResource(jid);

	bool notify = bestResource.name() == resource.name();

	resourcePool()->removeResource(jid, resource);

	bestResource = resourcePool()->bestResource(jid);

	if (notify)
		notifyAboutPresenceChanged(jid, bestResource.name() == JabberResourcePool::EmptyResource.name()
									? resource : bestResource);
}

void JabberProtocol::notifyAboutPresenceChanged(const XMPP::Jid &jid, const XMPP::Resource &resource)
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

}

#include "moc_jabber-protocol.cpp"
