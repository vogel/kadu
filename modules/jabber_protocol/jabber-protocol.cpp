/*
 * %kadu copyright begin%
 * Copyright 2009, 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009, 2009, 2010 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include "gui/windows/subscription-window.h"
#include "resource/jabber-resource-pool.h"
#include "utils/pep-manager.h"
#include "utils/server-info-manager.h"
#include "utils/vcard-factory.h"
#include "iris/filetransfer.h"
#include "iris/irisnetglobal.h"
#include "services/jabber-roster-service.h"
#include "services/jabber-subscription-service.h"
#include "iris-status-adapter.h"
#include "jabber-contact-details.h"
#include "jabber-protocol-factory.h"
#include "facebook-protocol-factory.h"
#include "gtalk-protocol-factory.h"
#include "jabber-url-handler.h"

#include "jabber-protocol.h"

bool JabberProtocol::ModuleUnloading = false;

int JabberProtocol::initModule()
{
	kdebugf();

	if (ProtocolsManager::instance()->hasProtocolFactory("jabber")
			|| ProtocolsManager::instance()->hasProtocolFactory("gtalk")
			|| ProtocolsManager::instance()->hasProtocolFactory("facebook"))
		return 0;

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

	ModuleUnloading = true;

	UrlHandlerManager::instance()->unregisterUrlHandler("Jabber");
	ProtocolsManager::instance()->unregisterProtocolFactory(JabberProtocolFactory::instance());
	ProtocolsManager::instance()->unregisterProtocolFactory(GTalkProtocolFactory::instance());
	ProtocolsManager::instance()->unregisterProtocolFactory(FacebookProtocolFactory::instance());

	XMPP::irisNetCleanup();

	qRemovePostRoutine(QCA::deinit);

	kdebugf2();
}

JabberProtocol::JabberProtocol(Account account, ProtocolFactory *factory) :
		Protocol(account, factory), JabberClient(0), ResourcePool(0), serverInfoManager(0), PepManager(0),
		ContactsListReadOnly(false)
{
	kdebugf();

	VCardFactory::createInstance(this);

	if (account.id().endsWith(QLatin1String("@chat.facebook.com")))
		setContactsListReadOnly(true);

	initializeJabberClient();

	CurrentAvatarService = new JabberAvatarService(account, this);
	CurrentChatService = new JabberChatService(this);
	CurrentChatStateService = new JabberChatStateService(this);
	CurrentContactPersonalInfoService = new JabberContactPersonalInfoService(this);
	CurrentFileTransferService = new JabberFileTransferService(this);
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

	JabberClient->disconnect();

	JabberClient->setOSName(SystemInfo::instance()->osFullName());
	JabberClient->setTimeZone(SystemInfo::instance()->timezone(), SystemInfo::instance()->timezoneOffset());
	JabberClient->setClientName("Kadu");
	JabberClient->setClientVersion(Core::instance()->version());

	// Set caps node information
	JabberClient->setCapsNode("http://psi-im.org/caps");
	JabberClient->setCapsVersion("0.12");

	JabberClient->setForceTLS(jabberAccountDetails->encryptionMode() != JabberAccountDetails::Encryption_No);

	// override server and port (this should be dropped when using the new protocol and no direct SSL)
	JabberClient->setUseSSL(jabberAccountDetails->encryptionMode() == JabberAccountDetails::Encryption_Legacy);
	JabberClient->setOverrideHost(jabberAccountDetails->useCustomHostPort(), jabberAccountDetails->customHost(), jabberAccountDetails->customPort());

	JabberClient->setFileTransfersEnabled(true); // i haz it
	jabberID = account().id();

	// PROXY CONNECTION
	// Cannot be used, because JabberClient->connect does its own Proxy stuff
	// that looks like it gets global Proxy settings, this code looks like it should
	// not be here, because JabberClient implements this already
	// TODO: ecleanup code

/*
	connector = JabberClient->clientConnector();
	XMPP::AdvancedConnector::Proxy p;

	AccountProxySettings proxySettings = account().proxySettings();
	if (proxySettings.enabled())
	{
		p.setHttpConnect(proxySettings.address(), proxySettings.port());
		if (proxySettings.requiresAuthentication() && !proxySettings.user().isEmpty())
			p.setUserPass(proxySettings.user(), proxySettings.password());
	}

	connector->setProxy(p);

	// END OF PROXY SETTINGS
	// THis is also implemented in JabberClient class

	if (confUseSSL && QCA::isSupported("tls"))
	{
		tls = new QCA::TLS;
		tls->setTrustedCertificates(CertUtil::allCertificates());
		tlsHandler = new XMPP::QCATLSHandler(tls);
		tlsHandler->setXMPPCertCheck(true);
		connect(tlsHandler, SIGNAL(tlsHandshaken()), SLOT(tlsHandshaken()));
	}

	connector->setOptHostPort(host, port);
	connector->setOptSSL(confUseSSL);

	stream = new XMPP::ClientStream(connector, tlsHandler);
*/

	JabberClient->setAllowPlainTextPassword(plainAuthToXMPP(jabberAccountDetails->plainAuthMode()));

	networkStateChanged(NetworkConnecting);
	jabberID = jabberID.withResource(jabberAccountDetails->resource());
	JabberClient->connect(jabberID, account().password(), true);

	// Initialize server info stuff
	serverInfoManager = new ServerInfoManager(JabberClient->client(), this);
	connect(serverInfoManager, SIGNAL(featuresChanged()),
		this, SLOT(serverFeaturesChanged()));

	// Initialize PubSub stuff
	PepManager = new PEPManager(JabberClient->client(), serverInfoManager, this);
	connect(PepManager, SIGNAL(itemPublished(const XMPP::Jid&, const QString&, const XMPP::PubSubItem&)),
		this, SLOT(itemPublished(const XMPP::Jid&, const QString&, const XMPP::PubSubItem&)));
	connect(PepManager, SIGNAL(itemRetracted(const XMPP::Jid&, const QString&, const XMPP::PubSubRetraction&)),
		this, SLOT(itemRetracted(const XMPP::Jid&, const QString&, const XMPP::PubSubRetraction&)));
	pepAvailable = false;

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

	changeStatus();
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

		/* Tell backend class to disconnect. */
		JabberClient->disconnect();
	}

	/* FIXME:
	 * We should delete the JabberClient instance here,
	 * but active timers in Iris prevent us from doing so.
	 * (in a failed connection attempt, these timers will
	 * try to access an already deleted object).
	 * Instead, the instance will lurk until the next
	 * connection attempt.
	 */
	kdebug("Disconnected.\n");

	if (serverInfoManager)
	{
		disconnect(serverInfoManager, SIGNAL(featuresChanged()),
			this, SLOT(serverFeaturesChanged()));
	}

	delete serverInfoManager;
	serverInfoManager = 0;

	if (!ModuleUnloading && PepManager)
	{
		delete PepManager;
		PepManager = 0;
		pepAvailable = false;
	}

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

	if (!nextStatus().isDisconnected()) // user still wants to login
		QTimer::singleShot(1000, this, SLOT(login())); // try again after one second
	else if (!nextStatus().isDisconnected())
		setStatus(Status());

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
	resourcePool()->addResource(jid, resource);

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

void JabberProtocol::addContactToRoster(Contact contact, bool requestAuth)
{
	if (!isConnected() || contact.contactAccount() != account() || contact.ownerBuddy().isAnonymous())
		return;

	Buddy buddy = contact.ownerBuddy();
	QStringList groupsList;

	foreach (Group group, buddy.groups())
		groupsList.append(group.name());

	//TODO last parameter: automagic authorization request - make it configurable
	JabberClient->addContact(contact.id(), buddy.display(), groupsList, requestAuth);
}

void JabberProtocol::contactAttached(Contact contact)
{
	return addContactToRoster(contact, true);
}

void JabberProtocol::contactDetached(Contact contact)
{
	if (!isConnected() || contact.contactAccount() != account())
		return;

	JabberClient->removeContact(contact.id());
}

void JabberProtocol::buddyUpdated(Buddy &buddy)
{
	if (!isConnected())
		return;

	QList<Contact> contacts = buddy.contacts(account());
	if (contacts.isEmpty() || buddy.isAnonymous())
		return;

	QStringList groupsList;
	foreach (Group group, buddy.groups())
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
	foreach (Group group, buddy.groups())
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

bool JabberProtocol::validateUserID(const QString& uid)
{
	XMPP::Jid j = XMPP::Jid(uid);
	return j.isValid();
}

JabberResourcePool *JabberProtocol::resourcePool()
{
	if (!ResourcePool)
		ResourcePool = new JabberResourcePool(this);
	return ResourcePool;
}

void JabberProtocol::changeStatus()
{
	Status newStatus = nextStatus();
	if (IrisStatusAdapter::statusesEqual(newStatus, status()))
		return;

	if (newStatus.isDisconnected() && status().isDisconnected())
	{
		networkStateChanged(NetworkDisconnected);

		setAllOffline();

		JabberClient->disconnect();

		if (newStatus.description() != status().description())
			statusChanged(newStatus);

		// i dont thing we need it
		// setStatus(newStatus());
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

void JabberProtocol::serverFeaturesChanged()
{
	if (serverInfoManager)
		setPEPAvailable(serverInfoManager->hasPEP());
}

void JabberProtocol::setPEPAvailable(bool b)
{
	if (pepAvailable == b)
		return;

	pepAvailable = b;

	// Publish support
	if (b && JabberClient->client()->extensions().contains("ep"))
	{
		QStringList pepNodes;
		/*pepNodes += "http://jabber.org/protocol/mood";
		pepNodes += "http://jabber.org/protocol/tune";
		pepNodes += "http://jabber.org/protocol/physloc";
		pepNodes += "http://jabber.org/protocol/geoloc";*/
		pepNodes += "http://www.xmpp.org/extensions/xep-0084.html#ns-data";
		pepNodes += "http://www.xmpp.org/extensions/xep-0084.html#ns-metadata";
		JabberClient->client()->addExtension("ep", XMPP::Features(pepNodes));
		//setStatusActual(d->loginStatus);
	}
	else if (!b && JabberClient->client()->extensions().contains("ep"))
	{
		JabberClient->client()->removeExtension("ep");
		//setStatusActual(d->loginStatus);
	}

	// Publish current tune information
// 	if (b && d->psi->tuneController() && d->options->getOption("options.extended-presence.tune.publish").toBool()) {
// 		Tune current = d->psi->tuneController()->currentTune();
// 		if (!current.isNull())
// 			publishTune(current);
// 	}
}

void JabberProtocol::itemPublished(const XMPP::Jid& j, const QString& n, const XMPP::PubSubItem& item)
{
	Q_UNUSED(j)
	Q_UNUSED(n)
	Q_UNUSED(item)
	/*
	// User Tune
	if (n == "http://jabber.org/protocol/tune") {
		// Parse tune
		QDomElement element = item.payload();
		QDomElement e;
		QString tune;
		bool found;

		e = findSubTag(element, "artist", &found);
		if (found)
			tune += e.text() + " - ";

		e = findSubTag(element, "title", &found);
		if (found)
			tune += e.text();

		foreach(UserListItem* u, findRelevant(j)) {
			// FIXME: try to find the right resource using JEP-33 'replyto'
			//UserResourceList::Iterator rit = u->userResourceList().find(<resource>);
			//bool found = (rit == u->userResourceList().end()) ? false: true;
			//if(found)
			//	(*rit).setTune(tune);
			u->setTune(tune);
			cpUpdate(*u);
		}
	}
	else if (n == "http://jabber.org/protocol/mood") {
		Mood mood(item.payload());
		foreach(UserListItem* u, findRelevant(j)) {
			u->setMood(mood);
			cpUpdate(*u);
		}
	}
	else if (n == "http://jabber.org/protocol/geoloc") {
		// FIXME: try to find the right resource using JEP-33 'replyto'
		// see tune case above
		GeoLocation geoloc(item.payload());
		foreach(UserListItem* u, findRelevant(j)) {
			u->setGeoLocation(geoloc);
			cpUpdate(*u);
		}
	}
	else if (n == "http://jabber.org/protocol/physloc") {
		// FIXME: try to find the right resource using JEP-33 'replyto'
		// see tune case above
		PhysicalLocation physloc(item.payload());
		foreach(UserListItem* u, findRelevant(j)) {
			u->setPhysicalLocation(physloc);
			cpUpdate(*u);
		}
	}
	*/
}

void JabberProtocol::itemRetracted(const XMPP::Jid& j, const QString& n, const XMPP::PubSubRetraction& item)
{
	Q_UNUSED(j)
	Q_UNUSED(n)
	Q_UNUSED(item)
	// User Tune
	/*if (n == "http://jabber.org/protocol/tune") {
		// Parse tune
		foreach(UserListItem* u, findRelevant(j)) {
			// FIXME: try to find the right resource using JEP-33 'replyto'
			//UserResourceList::Iterator rit = u->userResourceList().find(<resource>);
			//bool found = (rit == u->userResourceList().end()) ? false: true;
			//if(found)
			//	(*rit).setTune(tune);
			u->setTune(QString());
			cpUpdate(*u);
		}
	}
	else if (n == "http://jabber.org/protocol/mood") {
		foreach(UserListItem* u, findRelevant(j)) {
			u->setMood(Mood());
			cpUpdate(*u);
		}
	}
	else if (n == "http://jabber.org/protocol/geoloc") {
		// FIXME: try to find the right resource using JEP-33 'replyto'
		// see tune case above
		foreach(UserListItem* u, findRelevant(j)) {
			u->setGeoLocation(GeoLocation());
			cpUpdate(*u);
		}
	}
	else if (n == "http://jabber.org/protocol/physloc") {
		// FIXME: try to find the right resource using JEP-33 'replyto'
		// see tune case above
		foreach(UserListItem* u, findRelevant(j)) {
			u->setPhysicalLocation(PhysicalLocation());
			cpUpdate(*u);
		}
	}
	*/
}


