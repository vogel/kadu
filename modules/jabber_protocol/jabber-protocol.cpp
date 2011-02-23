/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Piotr Pełzowski (floss@pelzowski.eu)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2010 Bartłomiej Zimoń (uzi18@o2.pl)
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
#include "debug.h"
#include "gui/windows/message-dialog.h"
#include "gui/windows/password-window.h"
#include "os/generic/system-info.h"
#include "protocols/protocols-manager.h"
#include "status/status-type-manager.h"
#include "url-handlers/url-handler-manager.h"

#include "actions/jabber-actions.h"
#include "actions/jabber-protocol-menu-manager.h"
#include "certificates/trusted-certificates-manager.h"
#include "utils/vcard-factory.h"
#include "iris/irisnetglobal.h"
#include "services/jabber-roster-service.h"
#include "services/jabber-subscription-service.h"
#include "iris-status-adapter.h"
#include "jabber-contact-details.h"
#include "jabber-id-validator.h"
#include "jabber-protocol-factory.h"
#include "facebook-protocol-factory.h"
#include "gtalk-protocol-factory.h"
#include "jabber-url-handler.h"

#include "jabber-protocol.h"

int JabberProtocol::initModule()
{
	kdebugf();

	if (ProtocolsManager::instance()->hasProtocolFactory("jabber")
			|| ProtocolsManager::instance()->hasProtocolFactory("gtalk")
			|| ProtocolsManager::instance()->hasProtocolFactory("facebook"))
		return 0;

	JabberIdValidator::createInstance();
	VCardFactory::createInstance();

	JabberActions::registerActions();
	JabberProtocolMenuManager::createInstance();

	JabberProtocolFactory::createInstance();
	GTalkProtocolFactory::createInstance();
	FacebookProtocolFactory::createInstance();

	ProtocolsManager::instance()->registerProtocolFactory(JabberProtocolFactory::instance());
	ProtocolsManager::instance()->registerProtocolFactory(GTalkProtocolFactory::instance());
	ProtocolsManager::instance()->registerProtocolFactory(FacebookProtocolFactory::instance());

	UrlHandlerManager::instance()->registerUrlHandler("Jabber", new JabberUrlHandler());

	kdebugf2();
	return 0;
}

void JabberProtocol::closeModule()
{
	kdebugf();

	UrlHandlerManager::instance()->unregisterUrlHandler("Jabber");
	ProtocolsManager::instance()->unregisterProtocolFactory(JabberProtocolFactory::instance());
	ProtocolsManager::instance()->unregisterProtocolFactory(GTalkProtocolFactory::instance());
	ProtocolsManager::instance()->unregisterProtocolFactory(FacebookProtocolFactory::instance());

	JabberProtocolFactory::destroyInstance();
	GTalkProtocolFactory::destroyInstance();
	FacebookProtocolFactory::destroyInstance();

	JabberProtocolMenuManager::destroyInstance();
	JabberActions::unregisterActions();

	VCardFactory::destroyInstance();
	JabberIdValidator::destroyInstance();
	TrustedCertificatesManager::destroyInstance();
	XMPP::irisNetCleanup();

	qRemovePostRoutine(QCA::deinit);

	kdebugf2();
}

JabberProtocol::JabberProtocol(Account account, ProtocolFactory *factory) :
		Protocol(account, factory), JabberClient(0), ContactsListReadOnly(false)
{
	kdebugf();

	if (account.id().endsWith(QLatin1String("@chat.facebook.com")))
		setContactsListReadOnly(true);

	initializeJabberClient();

	CurrentAvatarService = new JabberAvatarService(account, this);
	CurrentChatService = new JabberChatService(this);
	CurrentChatStateService = new JabberChatStateService(this);
	CurrentContactPersonalInfoService = new JabberContactPersonalInfoService(this);
	CurrentPersonalInfoService = new JabberPersonalInfoService(this);
	CurrentRosterService = new JabberRosterService(this);
	connect(CurrentRosterService, SIGNAL(rosterDownloaded(bool)),
			this, SLOT(rosterDownloaded(bool)));
	CurrentSubscriptionService = new JabberSubscriptionService(this);

	connectContactManagerSignals();

	kdebugf2();
}

JabberProtocol::~JabberProtocol()
{
	disconnectContactManagerSignals();
	logout();
}

void JabberProtocol::connectContactManagerSignals()
{
	connect(ContactManager::instance(), SIGNAL(contactDetached(Contact)),
			this, SLOT(contactDetached(Contact)));
	connect(ContactManager::instance(), SIGNAL(contactAttached(Contact)),
			this, SLOT(contactAttached(Contact)));
	connect(ContactManager::instance(), SIGNAL(contactIdChanged(Contact, const QString &)),
			this, SLOT(contactIdChanged(Contact, const QString &)));
	connect(ContactManager::instance(), SIGNAL(contactReattached(Contact)),
			this, SLOT(contactUpdated(Contact)));

	connect(BuddyManager::instance(), SIGNAL(buddyUpdated(Buddy &)),
			this, SLOT(buddyUpdated(Buddy &)));
}

void JabberProtocol::disconnectContactManagerSignals()
{
	disconnect(ContactManager::instance(), SIGNAL(contactDetached(Contact)),
			this, SLOT(contactDetached(Contact)));
	disconnect(ContactManager::instance(), SIGNAL(contactAttached(Contact)),
			this, SLOT(contactAttached(Contact)));
	disconnect(ContactManager::instance(), SIGNAL(contactIdChanged(Contact, const QString &)),
			this, SLOT(contactIdChanged(Contact, const QString &)));
	disconnect(ContactManager::instance(), SIGNAL(contactReattached(Contact)),
			this, SLOT(contactUpdated(Contact)));

	disconnect(BuddyManager::instance(), SIGNAL(buddyUpdated(Buddy &)),
			this, SLOT(buddyUpdated(Buddy &)));
}

void JabberProtocol::setContactsListReadOnly(bool contactsListReadOnly)
{
	ContactsListReadOnly = contactsListReadOnly;
}

bool JabberProtocol::contactsListReadOnly()
{
	return ContactsListReadOnly;
}

void JabberProtocol::initializeJabberClient()
{
	JabberClient = new XMPP::JabberClient(this, this);

	connect(JabberClient, SIGNAL(csDisconnected()), this, SLOT(disconnectedFromServer()));
	connect(JabberClient, SIGNAL(connected()), this, SLOT(connectedToServer()));

	connect(JabberClient, SIGNAL(resourceAvailable(const XMPP::Jid &, const XMPP::Resource &)),
		   this, SLOT(clientResourceReceived(const XMPP::Jid &, const XMPP::Resource &)));
	connect(JabberClient, SIGNAL(resourceUnavailable(const XMPP::Jid &, const XMPP::Resource &)),
		   this, SLOT(clientResourceReceived(const XMPP::Jid &, const XMPP::Resource &)));

	connect(JabberClient, SIGNAL(connectionError(QString)), this, SLOT(connectionErrorSlot(QString)));
	connect(JabberClient, SIGNAL(invalidPassword()), this, SLOT(invalidPasswordSlot()));

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

void JabberProtocol::login(const QString &password, bool permanent)
{
	if (isConnected())
		return;
	
	if (password.isEmpty()) // user did not give us password, so prevent from further reconnecting
	{
		Status newstat = status();
		newstat.setType("Offline");
		setStatus(newstat);
		statusChanged(newstat);
		return;
	}

	account().setPassword(password);
	account().setRememberPassword(permanent);
	account().setHasPassword(!password.isEmpty());

	connectToServer();
}

void JabberProtocol::connectionErrorSlot(const QString& message)
{
	if (JabberClient && JabberClient->clientConnector())
		emit connectionError(account(), JabberClient->clientConnector()->host(), message);
}

void JabberProtocol::invalidPasswordSlot()
{
	emit invalidPassword(account());
}

void JabberProtocol::connectToServer()
{
	kdebugf();

	JabberAccountDetails *jabberAccountDetails = dynamic_cast<JabberAccountDetails *>(account().details());
	if (!jabberAccountDetails)
		return;

	if (account().id().isEmpty())
	{
		MessageDialog::show("dialog-warning", tr("Kadu"), tr("XMPP username is not set!"));
		setStatus(Status());
		statusChanged(Status());
		kdebugmf(KDEBUG_FUNCTION_END, "end: XMPP username is not set\n");
		return;
	}

	if (!account().hasPassword())
	{
		QString message = tr("Please provide password for %1 (%2) account")
				.arg(account().accountIdentity().name())
				.arg(account().id());
		PasswordWindow::getPassword(message, this, SLOT(login(const QString &, bool)));
		return;
	}

	JabberClient->setOSName(SystemInfo::instance()->osFullName());
	JabberClient->setTimeZone(SystemInfo::instance()->timezone(), SystemInfo::instance()->timezoneOffset());
	JabberClient->setClientName("Kadu");
	JabberClient->setClientVersion(Core::instance()->version());

	// Set caps node information
	JabberClient->setCapsNode("http://kadu.im/caps");
	JabberClient->setCapsVersion("0.7");

	JabberClient->setForceTLS(jabberAccountDetails->encryptionMode() != JabberAccountDetails::Encryption_No);

	// override server and port (this should be dropped when using the new protocol and no direct SSL)
	JabberClient->setUseSSL(jabberAccountDetails->encryptionMode() == JabberAccountDetails::Encryption_Legacy);
	JabberClient->setOverrideHost(jabberAccountDetails->useCustomHostPort(), jabberAccountDetails->customHost(), jabberAccountDetails->customPort());

//	JabberClient->setFileTransfersEnabled(true); // i haz it
	jabberID = account().id();

	JabberClient->setAllowPlainTextPassword(plainAuthToXMPP(jabberAccountDetails->plainAuthMode()));

	networkStateChanged(NetworkConnecting);
	jabberID = jabberID.withResource(jabberAccountDetails->resource());
	JabberClient->connect(jabberID, account().password(), true);

	kdebugf2();
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

void JabberProtocol::connectedToServer()
{
	kdebugf();

	networkStateChanged(NetworkConnected);

	// ask for roster
	CurrentRosterService->downloadRoster();
	kdebugf2();
}

void JabberProtocol::rosterDownloaded(bool success)
{
	Q_UNUSED(success)

	/* Since we are online now, set initial presence. Don't do this
	* before the roster request or we will receive presence
	* information before we have updated our roster with actual
	* contacts from the server! (Iris won't forward presence
	* information in that case either). */
	kdebug("Setting initial presence...\n");

	changeStatus(true);
}

// disconnect or stop reconnecting
void JabberProtocol::logout()
{
	kdebugf();

	Status newstat = status();
	if (!status().isDisconnected())
	{
		newstat.setType("Offline");
		setStatus(newstat);
	}

	disconnectFromServer(IrisStatusAdapter::toIrisStatus(newstat));
	setAllOffline();

	kdebugf2();
}

void JabberProtocol::disconnectFromServer(const XMPP::Status &s)
{
	kdebugf();

	if (isConnected())
	{
		kdebug("Still connected, closing connection...\n");
		// make sure that the connection animation gets stopped if we're still
		// in the process of connecting

		JabberClient->setPresence(s);
	}
	/* Tell backend class to disconnect. */
	JabberClient->disconnect();

	kdebug("Disconnected.\n");

	networkStateChanged(NetworkDisconnected);
	kdebugf2();
}

void JabberProtocol::slotClientDebugMessage(const QString &msg)
{
	Q_UNUSED(msg)

	kdebugm(KDEBUG_WARNING, "XMPP Client debug:  %s\n", qPrintable(msg));
}

void JabberProtocol::disconnectedFromServer()
{
	kdebugf();

	setAllOffline();

	networkStateChanged(NetworkDisconnected);

	JabberClient->disconnect();

	if (!nextStatus().isDisconnected()) // user still wants to login
		QTimer::singleShot(1000, this, SLOT(login())); // try again after one second

	kdebugf2();
}

void JabberProtocol::login()
{
	if (isConnected())
		return;
	connectToServer();
}

void JabberProtocol::clientResourceReceived(const XMPP::Jid &jid, const XMPP::Resource &resource)
{
	kdebugf();
	kdebug("New resource available for %s\n", jid.full().toLocal8Bit().data());
//	resourcePool()->addResource(jid, resource);

	Status status(IrisStatusAdapter::fromIrisStatus(resource.status()));
	Contact contact = ContactManager::instance()->byId(account(), jid.bare(), ActionReturnNull);

	if (contact)
	{
		Status oldStatus = contact.currentStatus();
		contact.setCurrentStatus(status);

		emit contactStatusChanged(contact, oldStatus);
	}
	kdebugf2();
}

void JabberProtocol::contactAttached(Contact contact)
{
	if (CurrentRosterService)
		CurrentRosterService->addContact(contact);
}

void JabberProtocol::contactDetached(Contact contact)
{
	if (CurrentRosterService)
		CurrentRosterService->removeContact(contact);
}

void JabberProtocol::buddyUpdated(Buddy &buddy)
{
	if (!isConnected())
		return;

	QList<Contact> contacts = buddy.contacts(account());
	if (contacts.isEmpty() || buddy.isAnonymous())
		return;

	QStringList groupsList;
	foreach (const Group &group, buddy.groups())
		groupsList.append(group.name());

	foreach (const Contact &contact, contacts)
		JabberClient->updateContact(contact.id(), buddy.display(), groupsList);
}

void JabberProtocol::contactUpdated(Contact contact)
{
	if (!isConnected() || contact.contactAccount() != account())
		return;

	Buddy buddy = contact.ownerBuddy();
	if (buddy.isAnonymous())
		return;

	QStringList groupsList;
	foreach (const Group &group, buddy.groups())
		groupsList.append(group.name());

	JabberClient->updateContact(contact.id(), buddy.display(), groupsList);
}

void JabberProtocol::contactIdChanged(Contact contact, const QString &oldId)
{
  	if (!isConnected() || contact.contactAccount() != account())
		return;

	JabberClient->removeContact(oldId);
	contactAttached(contact);
}

void JabberProtocol::changeStatus()
{
	changeStatus(false);
}

void JabberProtocol::changeStatus(bool force)
{
	Status newStatus = nextStatus();
	if (!force && IrisStatusAdapter::statusesEqual(newStatus, status()))
		return;

	if (newStatus.isDisconnected() && status().isDisconnected())
	{
		if (newStatus.description() != status().description())
			statusChanged(newStatus);

		if (NetworkConnecting == state())
			networkStateChanged(NetworkDisconnected);

		return;
	}

	if (isConnecting())
		return;

	if (!isConnected())
	{
		login();
		return;
	}

	XMPP::Status xmppStatus = IrisStatusAdapter::toIrisStatus(newStatus);
	JabberClient->setPresence(xmppStatus);

	if (newStatus.isDisconnected())
	{
		networkStateChanged(NetworkDisconnected);

		setAllOffline();

		JabberClient->disconnect();

		if (!nextStatus().isDisconnected())
			setStatus(Status());
	}

	statusChanged(IrisStatusAdapter::fromIrisStatus(xmppStatus));
}

void JabberProtocol::changePrivateMode()
{
	//changeStatus();
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
