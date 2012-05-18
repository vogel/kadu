/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009, 2010, 2010, 2010, 2011 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Piotr Pełzowski (floss@pelzowski.eu)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2009, 2010 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QCoreApplication>
#include <QtCrypto>

#include "buddies/buddy-manager.h"
#include "buddies/group-manager.h"
#include "contacts/contact-manager.h"
#include "core/core.h"
#include "gui/windows/message-dialog.h"
#include "gui/windows/password-window.h"
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
#include "services/jabber-roster-service.h"
#include "services/jabber-subscription-service.h"
#include "utils/vcard-factory.h"
#include "facebook-protocol-factory.h"
#include "gtalk-protocol-factory.h"
#include "iris-status-adapter.h"
#include "jabber-contact-details.h"
#include "jabber-id-validator.h"
#include "jabber-protocol-factory.h"
#include "jabber-url-handler.h"

#include "jabber-protocol.h"

JabberProtocol::JabberProtocol(Account account, ProtocolFactory *factory) :
		Protocol(account, factory), JabberClient(0), ResourcePool(0),
		ContactsListReadOnly(false)
{
	kdebugf();

	if (account.id().endsWith(QLatin1String("@chat.facebook.com")))
		setContactsListReadOnly(true);

	initializeJabberClient();

	CurrentAvatarService = new JabberAvatarService(account, this);
	XMPP::JabberChatService *chatService = new XMPP::JabberChatService(this);
	XMPP::JabberChatStateService *chatStateService = new XMPP::JabberChatStateService(this);
	CurrentContactPersonalInfoService = new JabberContactPersonalInfoService(this);
	CurrentFileTransferService = new JabberFileTransferService(this);
	CurrentPersonalInfoService = new JabberPersonalInfoService(this);

	connect(xmppClient(), SIGNAL(messageReceived(const Message &)),
	        chatService, SLOT(handleReceivedMessage(Message)));
	connect(xmppClient(), SIGNAL(messageReceived(const Message &)),
	        chatStateService, SLOT(handleReceivedMessage(const Message &)));
	connect(chatService, SIGNAL(messageAboutToSend(Message&)),
	        chatStateService, SLOT(handleMessageAboutToSend(Message&)));

	XMPP::JabberRosterService *rosterService = new XMPP::JabberRosterService(this);

	chatService->setClient(JabberClient->client());
	chatStateService->setClient(JabberClient->client());
	rosterService->setClient(JabberClient->client());

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

void JabberProtocol::initializeJabberClient()
{
	JabberClient = new XMPP::JabberClient(this, this);

	connect(JabberClient->client(), SIGNAL(disconnected()), this, SLOT(connectionError()));
	connect(JabberClient, SIGNAL(connected()), this, SLOT(connectedToServer()));

	connect(JabberClient, SIGNAL(resourceAvailable(const XMPP::Jid &, const XMPP::Resource &)),
		   this, SLOT(clientAvailableResourceReceived(const XMPP::Jid &, const XMPP::Resource &)));
	connect(JabberClient, SIGNAL(resourceUnavailable(const XMPP::Jid &, const XMPP::Resource &)),
		   this, SLOT(clientUnavailableResourceReceived(const XMPP::Jid &, const XMPP::Resource &)));

	connect(JabberClient, SIGNAL(connectionError(QString)), this, SLOT(connectionErrorSlot(QString)));
	connect(JabberClient, SIGNAL(invalidPassword()), this, SLOT(passwordRequired()));

		/*//TODO: implement in the future
		connect( JabberClient, SIGNAL ( groupChatJoined ( const XMPP::Jid & ) ),
				   this, SLOT ( slotGroupChatJoined ( const XMPP::Jid & ) ) );
		connect( JabberClient, SIGNAL ( groupChatLeft ( const XMPP::Jid & ) ),
				   this, SLOT ( slotGroupChatLeft ( const XMPP::Jid & ) ) );
		connect( JabberClient, SIGNAL ( groupChatPresence ( const XMPP::Jid &, const XMPP::Status & ) ),
				   this, SLOT ( slotGroupChatPresence ( const XMPP::Jid &, const XMPP::Status & ) ) );
		connect( JabberClient, SIGNAL ( groupChatError ( const XMPP::Jid &, int, const QString & ) ),
				   this, SLOT ( slotGroupChatError ( const XMPP::Jid &, int, const QString & ) ) );
		*/
	connect(JabberClient, SIGNAL( debugMessage(const QString &)),
		   this, SLOT(slotClientDebugMessage(const QString &)));
}

void JabberProtocol::connectionErrorSlot(const QString& message)
{
	if (JabberClient && JabberClient->clientConnector())
		emit connectionError(account(), JabberClient->clientConnector()->host(), message);
}

XMPP::ClientStream::AllowPlainType JabberProtocol::plainAuthToXMPP(JabberAccountDetails::AllowPlainType type)
{
	if (type == JabberAccountDetails::NoAllowPlain)
		return XMPP::ClientStream::NoAllowPlain;
	if (type == JabberAccountDetails::AllowPlain)
		return XMPP::ClientStream::AllowPlain;
	else
		return XMPP::ClientStream::AllowPlainOverTLS;
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
}

void JabberProtocol::disconnectFromServer(const XMPP::Status &s)
{
	kdebugf();

	if (JabberClient->client())
	{
		XMPP::Status status = s;
		/* Tell backend class to disconnect. */
		JabberClient->disconnect(status);
	}

	kdebug("Disconnected.\n");

	// in state machine?
// 	machine()->loggedOut();
	kdebugf2();
}

void JabberProtocol::slotClientDebugMessage(const QString &msg)
{
	Q_UNUSED(msg)

	kdebugm(KDEBUG_WARNING, "XMPP Client debug:  %s\n", qPrintable(msg));
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
		JabberClient->setOSName(SystemInfo::instance()->osFullName());
		JabberClient->setClientName("Kadu");
		JabberClient->setClientVersion(Core::instance()->version());
		kdebugm(KDEBUG_WARNING, "CLIENT:  %s, %s\n", qPrintable(SystemInfo::instance()->osFullName()), qPrintable(Core::instance()->version()));
	}

	// Set caps node information
	JabberClient->setCapsNode("http://kadu.im/caps");
	JabberClient->setCapsVersion("0.10");

	JabberClient->setForceTLS(jabberAccountDetails->encryptionMode() != JabberAccountDetails::Encryption_No);

	// override server and port (this should be dropped when using the new protocol and no direct SSL)
	JabberClient->setUseSSL(jabberAccountDetails->encryptionMode() == JabberAccountDetails::Encryption_Legacy);
	JabberClient->setOverrideHost(jabberAccountDetails->useCustomHostPort(), jabberAccountDetails->customHost(), jabberAccountDetails->customPort());

	jabberID = account().id();

	JabberClient->setAllowPlainTextPassword(plainAuthToXMPP(jabberAccountDetails->plainAuthMode()));

	jabberID = jabberID.withResource(jabberAccountDetails->resource());
	JabberClient->connect(jabberID, account().password(), true);

	kdebugf2();
}

void JabberProtocol::reconnect()
{
	login();
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
	connect(JabberClient, SIGNAL(csDisconnected()), this, SLOT(disconnectedFromServer()));

	rosterService()->prepareRoster(ContactManager::instance()->contacts(account(), ContactManager::ExcludeAnonymous));
}

void JabberProtocol::disconnectedFromServer()
{
	kdebugf();

	loggedOut();

	disconnect(JabberClient, SIGNAL(csDisconnected()), this, SLOT(disconnectedFromServer()));

	kdebugf2();
}

void JabberProtocol::logout()
{
	disconnectFromServer(IrisStatusAdapter::toIrisStatus(status()));
	loggedOut();
}

void JabberProtocol::sendStatusToServer()
{
	if (!isConnected() && !isDisconnecting())
		return;

	JabberClient->setPresence(IrisStatusAdapter::toIrisStatus(status()));
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
	Status status(IrisStatusAdapter::fromIrisStatus(resource.status()));
	Contact contact = ContactManager::instance()->byId(account(), jid.bare(), ActionReturnNull);

	if (!contact)
		return;

	Status oldStatus = contact.currentStatus();
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
