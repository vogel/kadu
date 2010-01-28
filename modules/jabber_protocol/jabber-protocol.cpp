/*
 * %kadu copyright begin%
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@go2.pl)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include <QtGui/QCheckBox>
#include <QtGui/QMessageBox>
#include <QtCrypto>

#include "libiris/include/filetransfer.h"
#include <xmpp.h>
#include <xmpp_tasks.h>

#include "accounts/account-manager.h"
#include "buddies/buddy-manager.h"
#include "buddies/group.h"
#include "buddies/group-manager.h"
#include "configuration/configuration-file.h"
#include "contacts/contact-manager.h"
#include "core/core.h"
#include "file-transfer/file-transfer-manager.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/message-dialog.h"
#include "gui/windows/password-window.h"
#include "gui/windows/subscription-window.h"
#include "gui/windows/main-configuration-window.h"

#include "debug.h"
#include "exports.h"
#include "icons-manager.h"
#include "kadu-config.h"
#include "misc/misc.h"
#include "protocols/protocol-menu-manager.h"
#include "status/status.h"

#include "file-transfer/jabber-file-transfer-handler.h"
#include "jabber-account-details.h"
#include "jabber-protocol.h"
#include "jabber-protocol-factory.h"
#include "os/generic/system-info.h"

extern "C" KADU_EXPORT int jabber_protocol_init()
{
	return JabberProtocol::initModule();
}

extern "C" KADU_EXPORT void jabber_protocol_close()
{
	JabberProtocol::closeModule();
}

int JabberProtocol::initModule()
{
	kdebugf();

	if (ProtocolsManager::instance()->hasProtocolFactory("jabber"))
		return 0;

	ProtocolsManager::instance()->registerProtocolFactory(JabberProtocolFactory::instance());

	kdebugf2();
	return 0;
}

void JabberProtocol::closeModule()
{
	kdebugf();
	ProtocolsManager::instance()->unregisterProtocolFactory(JabberProtocolFactory::instance());
	kdebugf2();
}

JabberProtocol::JabberProtocol(Account account, ProtocolFactory *factory) :
		Protocol(account, factory), JabberClient(NULL), ResourcePool(0)
{
	kdebugf();

	initializeJabberClient();

	CurrentChatService = new JabberChatService(this);
	CurrentChatStateService = new JabberChatStateService(this);
	CurrentFileTransferService = new JabberFileTransferService(this);
	CurrentAvatarService = new JabberAvatarService(this);

	connect(ContactManager::instance(), SIGNAL(contactDetached(Contact)),
			this, SLOT(contactDetached(Contact)));
	connect(ContactManager::instance(), SIGNAL(contactAttached(Contact)),
			this, SLOT(contactAttached(Contact)));
	connect(ContactManager::instance(), SIGNAL(contactIdChanged(Contact, const QString &)),
			this, SLOT(contactIdChanged(Contact, const QString &)));
	
	connect(BuddyManager::instance(), SIGNAL(buddyUpdated(Buddy &)),
			this, SLOT(buddyUpdated(Buddy &)));
		
	kdebugf2();
}

JabberProtocol::~JabberProtocol()
{
	disconnect(ContactManager::instance(), SIGNAL(contactDetached(Contact)),
			this, SLOT(contactDetached(Contact)));
	disconnect(ContactManager::instance(), SIGNAL(contactAttached(Contact)),
			this, SLOT(contactAttached(Contact)));
	disconnect(ContactManager::instance(), SIGNAL(contactIdChanged(Contact, const QString &)),
			this, SLOT(contactIdChanged(Contact, const QString &)));

	disconnect(BuddyManager::instance(), SIGNAL(buddyUpdated(Buddy &)),
			this, SLOT(buddyUpdated(Buddy &)));

	logout();
}

void JabberProtocol::initializeJabberClient()
{
	JabberClient = new XMPP::JabberClient(this);
	connect(JabberClient, SIGNAL(csDisconnected()), this, SLOT(disconnectedFromServer()));
	connect(JabberClient, SIGNAL(connected()), this, SLOT(connectedToServer()));

	connect(JabberClient, SIGNAL(subscription(const XMPP::Jid &, const QString &)),
		   this, SLOT(slotSubscription(const XMPP::Jid &, const QString &)));
	connect(JabberClient, SIGNAL(newContact(const XMPP::RosterItem &)),
		   this, SLOT(slotContactUpdated(const XMPP::RosterItem &)));
	connect(JabberClient, SIGNAL(contactUpdated(const XMPP::RosterItem &)),
		   this, SLOT(slotContactUpdated(const XMPP::RosterItem &)));
	connect(JabberClient, SIGNAL(contactDeleted(const XMPP::RosterItem &)),
		   this, SLOT(slotContactDeleted(const XMPP::RosterItem &)));
	connect(JabberClient, SIGNAL(rosterRequestFinished(bool)),
		   this, SLOT(rosterRequestFinished(bool)));

	connect(JabberClient, SIGNAL(resourceAvailable(const XMPP::Jid &, const XMPP::Resource &)),
		   this, SLOT(clientResourceReceived(const XMPP::Jid &, const XMPP::Resource &)));
	connect(JabberClient, SIGNAL(resourceUnavailable(const XMPP::Jid &, const XMPP::Resource &)),
		   this, SLOT(clientResourceReceived(const XMPP::Jid &, const XMPP::Resource &)));

	connect(JabberClient, SIGNAL(incomingFileTransfer()), this, SLOT(slotIncomingFileTransfer()));

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

void JabberProtocol::connectToServer()
{
	kdebugf();

	JabberAccountDetails *jabberAccountDetails = dynamic_cast<JabberAccountDetails *>(account().details());
	if (!jabberAccountDetails)
		return;

	if (account().id().isEmpty())
	{
		MessageDialog::msg(tr("Jabber ID not set!"), false, "Warning");
		setStatus(Status());
		kdebugmf(KDEBUG_FUNCTION_END, "end: Jabber ID not set\n");
		return;
	}

	if (!account().hasPassword())
	{
		PasswordWindow::getPassword(tr("Please provide password for %1 account").arg(account().name()),
				this, SLOT(login(const QString &, bool)));
		return;
	}

	JabberClient->disconnect();

	JabberClient->setOSName(SystemInfo::instance()->osFullName());
	JabberClient->setTimeZone(SystemInfo::instance()->timezone(), SystemInfo::instance()->timezoneOffset());
	JabberClient->setClientName("Kadu");
	JabberClient->setClientVersion(VERSION);

	// Set caps node information
	JabberClient->setCapsNode("http://psi-im.org/caps");
	JabberClient->setCapsVersion("0.12");

	JabberClient->setForceTLS(jabberAccountDetails->encryptionMode() != JabberAccountDetails::Encryption_No);

	// override server and port (this should be dropped when using the new protocol and no direct SSL)
	JabberClient->setUseSSL(jabberAccountDetails->encryptionMode() == JabberAccountDetails::Encryption_Legacy);
	JabberClient->setOverrideHost(jabberAccountDetails->useCustomHostPort(), jabberAccountDetails->customHost(), jabberAccountDetails->customPort());

	JabberClient->setFileTransfersEnabled(true); // i haz it
	rosterRequestDone = false;
	jabberID = account().id();

/*
//TODO: do nowej klasy dostosowa?
	XMPP::AdvancedConnector::Proxy p;
	if(config_file.readBoolEntry("Network", "UseProxy"))
	{
		p.setHttpConnect(config_file.readEntry("Network", "ProxyHost"), config_file.readNumEntry("Network", "ProxyPort"));
		if (!config_file.readEntry("Network", "ProxyUser").isEmpty())
			p.setUserPass(config_file.readEntry("Network", "ProxyUser"), config_file.readEntry("Network", "ProxyPassword"));
	}
	connector = new XMPP::AdvancedConnector;
	if(confUseSSL && QCA::isSupported("tls"))
	{
		tls = new QCA::TLS;
		tls->setTrustedCertificates(CertUtil::allCertificates());
		tlsHandler = new XMPP::QCATLSHandler(tls);
		tlsHandler->setXMPPCertCheck(true);
		connect(tlsHandler, SIGNAL(tlsHandshaken()), SLOT(tlsHandshaken()));
	}
	connector->setProxy(p);
	connector->setOptHostPort(host, port);
	connector->setOptSSL(confUseSSL);

	stream = new XMPP::ClientStream(connector, tlsHandler);
	stream->setAllowPlain(XMPP::ClientStream::AllowPlainOverTLS);

*/
	whileConnecting = true;
	networkStateChanged(NetworkConnecting);
	jabberID = jabberID.withResource(jabberAccountDetails->resource());
	networkStateChanged(NetworkConnecting);
	JabberClient->connect(jabberID, account().password(), true);
	kdebugf2();
}

void JabberProtocol::connectedToServer()
{
	kdebugf();

	whileConnecting = false;
	//crash by�... gdzie indziej status ustawia�
	///setStatus(Status::Online);
	networkStateChanged(NetworkConnected);
	//po zalogowaniu pobierz roster
	JabberClient->requestRoster();
	kdebugf2();
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

	disconnectFromServer(toXMPPStatus(newstat));
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
	networkStateChanged(NetworkDisconnected);
	kdebugf2();
}

void JabberProtocol::slotClientDebugMessage(const QString &msg)
{
	kdebugm(KDEBUG_WARNING, "Jabber Client debug:  %s\n", qPrintable(msg));
}

void JabberProtocol::rosterRequestFinished(bool success)
{
	kdebugf();
	if (success)
	{
		// the roster was imported successfully, clear
		// all "dirty" items from the contact list
		///contactPool()->cleanUp ();
	}
	rosterRequestDone = true;

	/* Since we are online now, set initial presence. Don't do this
	* before the roster request or we will receive presence
	* information before we have updated our roster with actual
	* contacts from the server! (Iris won't forward presence
	* information in that case either). */
	kdebug("Setting initial presence...\n");

	changeStatus(nextStatus());

	kdebugf2();
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

XMPP::Status JabberProtocol::toXMPPStatus(Status status)
{
	XMPP::Status s = XMPP::Status();
	const QString &type = status.type();

	if ("Online" == type)
		s.setType(XMPP::Status::Online);
	else if ("FreeForChat" == type)
		s.setType(XMPP::Status::FFC);
	else if ("DoNotDisturb" == type)
		s.setType(XMPP::Status::DND);
	else if ("NotAvailable" == type)
		s.setType(XMPP::Status::XA);
	else if ("Away" == type)
		s.setType(XMPP::Status::Away);
	else if ("Invisible" == type)
		s.setType(XMPP::Status::Invisible);
	else
		s.setType(XMPP::Status::Offline);

	s.setStatus(status.description());
	return s;
}

Status JabberProtocol::toStatus(XMPP::Status status)
{
	Status newstatus;
	if (status.isAvailable())
		newstatus.setType("Online");
	else if (status.isInvisible())
		newstatus.setType("Invisible");
	else
		newstatus.setType("Offline");

	if (status.show() == "away")
		newstatus.setType("Away");
	else if (status.show() == "xa")
		newstatus.setType("NotAvailable");
	else if (status.show() == "dnd")
		newstatus.setType("DoNotDisturb");
	else if (status.show() == "chat")
		newstatus.setType("FreeForChat");

	QString description = status.status();
	description.replace("\r\n", "\n");
	description.replace("\r", "\n");
	newstatus.setDescription(description);

	return newstatus;
}

void JabberProtocol::changeStatus(Status status)
{
	JabberClient->setPresence(toXMPPStatus(status));

	if (status.isDisconnected())
	{
		networkStateChanged(NetworkDisconnected);

		setAllOffline();

		if (!nextStatus().isDisconnected())
			setStatus(Status());
	}

	statusChanged(status);
}

void JabberProtocol::slotIncomingFileTransfer()
{
	XMPP::FileTransfer *jTransfer = client()->fileTransferManager()->takeIncoming();
	if (!jTransfer)
		return;

	Contact peer = ContactManager::instance()->byId(account(), jTransfer->peer().bare(), ActionCreateAndAdd);
	FileTransfer transfer = FileTransfer::create();
	transfer.setPeer(peer);
	transfer.setTransferType(TypeReceive);
	transfer.setRemoteFileName(jTransfer->fileName());

	transfer.createHandler();

	JabberFileTransferHandler *handler = dynamic_cast<JabberFileTransferHandler *>(transfer.handler());
	if (handler)
		handler->setJTransfer(jTransfer);

	CurrentFileTransferService->incomingFile(transfer);
}

void JabberProtocol::clientResourceReceived(const XMPP::Jid &jid, const XMPP::Resource &resource)
{
	kdebugf();
	kdebug("New resource available for %s\n", jid.full().toLocal8Bit().data());
	resourcePool()->addResource(jid, resource);

	Status status(toStatus(resource.status()));
	Contact contact = ContactManager::instance()->byId(account(), jid.bare(), ActionCreateAndAdd);

	Status oldStatus = contact.currentStatus();
	contact.setCurrentStatus(status);

	emit contactStatusChanged(contact, oldStatus);
	kdebugf2();
}

void JabberProtocol::contactAttached(Contact contact)
{
	if (!isConnected() || contact.contactAccount() != account())
		return;

	Buddy buddy = contact.ownerBuddy();
	QStringList groupsList;

	foreach (Group group, buddy.groups())
		groupsList.append(group.name());
	
	//TODO last parameter: automagic authorization request - make it configurable
	JabberClient->addContact(contact.id(), buddy.display(), groupsList, true);
}

void JabberProtocol::contactDetached(Contact contact)
{
	if (!isConnected() || contact.contactAccount() != account())
		return;

	JabberClient->removeContact(contact.id());
}

void JabberProtocol::buddyUpdated(Buddy &buddy)
{
	QList<Contact> contacts = buddy.contacts(account());
	if (contacts.isEmpty() || buddy.isAnonymous())
		return;

	QStringList groupsList;
	foreach (Group group, buddy.groups())
		groupsList.append(group.name());

	foreach (const Contact &contact, contacts)
		JabberClient->updateContact(contact.id(), buddy.display(), groupsList);
}

void JabberProtocol::contactUpdated(Contact &contact)
{
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

void JabberProtocol::slotContactUpdated(const XMPP::RosterItem &item)
{
	kdebugf();
	/**
	 * Subscription types are: Both, From, To, Remove, None.
	 * Both:   Both sides have authed each other, each side
	 *         can see each other's presence
	 * From:   The other side can see us.
	 * To:     We can see the other side. (implies we are
	 *         authed)
	 * Remove: Other side revoked our subscription request.
	 *         Not to be handled here.
	 * None:   No subscription.
	 *
	 * Regardless of the subscription type, we have to add
	 * a roster item here.
	 */

	kdebug("New roster item: %s (Subscription: %s )\n", item.jid().full().toLocal8Bit().data(), item.subscription().toString().toLocal8Bit().data());

	/*
	 * See if the contact need to be added, according to the criterias of
	 *  JEP-0162: Best Practices for Roster and Subscription Management
	 * http://www.jabber.org/jeps/jep-0162.html#contacts
	 */
	bool need_to_add=false;
	if (item.subscription().type() == XMPP::Subscription::Both || item.subscription().type() == XMPP::Subscription::To)
		need_to_add = true;
	else if (!item.ask().isEmpty())
		need_to_add = true;
	else if (!item.name().isEmpty() || !item.groups().isEmpty())
		need_to_add = true;

	if (item.jid().bare() == jabberID.bare())
	{
		// don't let remove the gateway contact, eh!
		need_to_add = true;
	}

	if (need_to_add)
	{
		/*
		 * See if the contact is already on our contact list
		 * if not add contact to our list
		 */
		Contact contact = ContactManager::instance()->byId(account(), item.jid().bare(), ActionCreateAndAdd);
		Buddy buddy = BuddyManager::instance()->byContact(contact, ActionCreateAndAdd);

		// if contact has name set it to display
		if (!item.name().isNull())
		{
			if (item.name() != buddy.display())
				buddy.setDisplay(item.name());
		}
		else
			buddy.setDisplay(item.jid().bare());

		if (buddy.isAnonymous()) // always false!!
		{
			// TODO: add some logic here?
			buddy.setAnonymous(false);

			GroupManager *gm = GroupManager::instance();
			// add this contact to all groups the contact is a member of
			foreach (QString group, item.groups())
				buddy.addToGroup(gm->byName(group,true /* create group */));
		}
		else
		{
			//TODO: synchronize groups
		}

		/*
		* Add / update the contact in contact list. In case the contact is already there,
		* it will be updated. In case the contact is not there yet, it
		* will be added to it.
		*/
		///JabberContact contact = contactPool()->addContact ( item, metaContact, false );

		/*
		* Set authorization property
		*/
		/**if ( !item.ask().isEmpty () )
// 		{
// 			contact->setProperty ( protocol()->propAuthorizationStatus, i18n ( "Waiting for authorization" ) );
// 		}
// 		else
// 		{
// 			contact->removeProperty ( protocol()->propAuthorizationStatus );
// 		}*/
	}
//	else if (!c.isAnonymous())  //we don't need to add it, and it is in the contact list
//	{
// 		Kopete::MetaContactmetaContact=c->metaContact();
// 		if(metaContact->isTemporary())
// 			return;
// 		kDebug (JABBER_DEBUG_GLOBAL) << c->contactId() <<
// 				" is on the contact list while it should not.  we are removing it.  - " << c << endl;
// 		delete c;
// 		if(metaContact->contacts().isEmpty())
// 			Kopete::ContactList::self()->removeMetaContact( metaContact );
//	}

	kdebugf2();
}

void JabberProtocol::slotContactDeleted(const XMPP::RosterItem &item)
{
	kdebug("Deleting contact %s", item.jid().bare().toLocal8Bit().data());
	Contact contact = ContactManager::instance()->byId(account(), item.jid().bare(), ActionReturnNull);
	if (contact)
	{
	  	Buddy owner = contact.ownerBuddy();
		contact.setOwnerBuddy(Buddy::null);
		if (owner.contacts().size() == 0)
			BuddyManager::instance()->removeItem(owner);
		
	}
}

void JabberProtocol::slotSubscription(const XMPP::Jid & jid, const QString &type)
{
	if (type == "unsubscribed")
	{
		/*
		 * Someone else removed our authorization to see them.
		 */
		kdebug("%s revoked our presence authorization", jid.full().toLocal8Bit().data());

		XMPP::JT_Roster *task;
		if (MessageDialog::ask(tr("The user %1 removed subscription to you. "
								   "You will no longer be able to view his/her online/offline status. "
								   "Do you want to delete the contact?").arg(jid.full())))
		{
			/*
			 * Delete this contact from our roster.
			 */
			task = new XMPP::JT_Roster(JabberClient->rootTask());
			task->remove(jid);
			task->go(true);

			Contact contact = ContactManager::instance()->byId(account(), jid.bare(), ActionReturnNull);
			if (contact)
			{
				Buddy owner = contact.ownerBuddy();
				contact.setOwnerBuddy(Buddy::null);
				if (owner.contacts().size() == 0)
					BuddyManager::instance()->removeItem(owner);
				
			}
		}
		else
			/*
				 * We want to leave the contact in our contact list.
				 * In this case, we need to delete all the resources
				 * we have for it, as the Jabber server won't signal us
				 * that the contact is offline now.
			*/
		resourcePool()->removeAllResources(jid);
	}

	if (type == "subscribe")
	{
		Contact contact = ContactManager::instance()->byId(account(), jid.bare(), ActionCreate);
		SubscriptionWindow::getSubscription(contact, this, SLOT(authorizeContact(Contact, bool)));
	}
	else if (type == "subscribed")
		MessageDialog::msg(QString("You are authorized by %1").arg(jid.bare()), false, "Warning");
	else if (type == "unsubscribe")
		MessageDialog::msg(QString("Contact %1 has removed authorization for you.").arg(jid.bare()), false, "Warning");
		//TODO: usuwa� kontakt z listy... ta, chyba tak

}

void JabberProtocol::authorizeContact(Contact contact, bool authorized)
{
	const XMPP::Jid jid = XMPP::Jid(contact.id());
	
	if (authorized)
	{
		JabberClient->resendSubscription(jid);
	}
	else
		JabberClient->rejectSubscription(jid);
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

	if (newStatus.isDisconnected() && status().isDisconnected())
	{
		networkStateChanged(NetworkDisconnected);

		setAllOffline();

		if (!nextStatus().isDisconnected())
			setStatus(Status());
		return;
	}

	if (NetworkConnecting == state())
		return;

	if (status().isDisconnected())
	{
		login();
		return;
	}

	changeStatus(newStatus);
}

void JabberProtocol::changePrivateMode()
{
	changeStatus();
}

QPixmap JabberProtocol::statusPixmap(Status status)
{
	QString pixmapName(dataPath("kadu/modules/data/jabber_protocol/"));

	QString groupName = status.type();

	if ("Online" == groupName)
		pixmapName.append("online");

	else if ("FreeForChat" == groupName)
		pixmapName.append("ffc");

	else if ("DoNotDisturb" == groupName)
		pixmapName.append("dnd");

	else if ("Away" == groupName)
		pixmapName.append("away");

	else if ("NotAvailable" == groupName)
		pixmapName.append("xa");

	else if ("Invisible" == groupName)
		pixmapName.append("invisible");

	else	pixmapName.append("offline");

	pixmapName.append(".png");

	return IconsManager::instance()->loadPixmap(pixmapName);
}

QPixmap JabberProtocol::statusPixmap(const QString &statusType)
{
	QString pixmapName(dataPath("kadu/modules/data/jabber_protocol/"));

	if ("Online" == statusType)
		pixmapName.append("online");

	else if ("FreeForChat" == statusType)
		pixmapName.append("ffc");

	else if ("DoNotDisturb" == statusType)
		pixmapName.append("dnd");

	else if ("Away" == statusType)
		pixmapName.append("away");

	else if ("NotAvailable" == statusType)
		pixmapName.append("xa");

	else if ("Invisible" == statusType)
		pixmapName.append("invisible");

	else	pixmapName.append("offline");

	pixmapName.append(".png");

	return IconsManager::instance()->loadPixmap(pixmapName);
}

JabberContactDetails * JabberProtocol::jabberContactDetails(Contact contact) const
{
	if (contact.isNull())
		return 0;
	return dynamic_cast<JabberContactDetails *>(contact.details());
}
