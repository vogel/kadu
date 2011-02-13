/*
 * %kadu copyright begin%
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
#include <stdio.h>
#include <qcheckbox.h>
#include <qtimer.h>
#include <iostream>
#include <qmessagebox.h>
#include <qregexp.h>
#include <QHostInfo>
#include <QtGui/QPushButton>
#include <QtGui/QMessageBox>

#include "buddies/buddy-manager.h"

#include "contacts/contact-manager.h"
#include "contacts/contact-set.h"

#include "buddies/group.h"
#include "buddies/group-manager.h"

#include "gui/windows/kadu-window.h"
#include "gui/windows/message-dialog.h"
#include "gui/windows/password-window.h"

#include "chat/message/message.h"
#include "chat/chat-manager.h"
#include "debug.h"
#include "icons-manager.h"
#include "status/status.h"
#include "status/status-type-manager.h"
#include "misc/misc.h"
#include "html_document.h"

#include "tlen.h"

#include "tlen-account-details.h"
#include "tlen-contact-details.h"
#include "tlen-protocol-factory.h"

#include "tlen-protocol.h"

#include "exports.h"

extern "C" KADU_EXPORT int tlen_protocol_init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	return TlenProtocol::initModule();
}

extern "C" KADU_EXPORT void tlen_protocol_close()
{
	TlenProtocol::closeModule();
}

int TlenProtocol::initModule()
{
	kdebugf();

	if (ProtocolsManager::instance()->hasProtocolFactory("tlen"))
		return 0;
	
	TlenProtocolFactory::createInstance();

	ProtocolsManager::instance()->registerProtocolFactory(TlenProtocolFactory::instance());

	kdebugf2();
	return 0;
}

void TlenProtocol::closeModule()
{
	kdebugf();

	ProtocolsManager::instance()->unregisterProtocolFactory(TlenProtocolFactory::instance());
	
	TlenProtocolFactory::destroyInstance();

	kdebugf2();
}

TlenProtocol::TlenProtocol(Account account, ProtocolFactory *factory): Protocol(account,factory), TlenClient(0)
{
	kdebugf();

	CurrentChatService = new TlenChatService(this);
	CurrentAvatarService = new TlenAvatarService(account, this);
	CurrentPersonalInfoService = new TlenPersonalInfoService(this);

// 	connect(ContactManager::instance(), SIGNAL(contactAboutToBeAdded(Contact)),
// 			this, SLOT(contactAboutToBeAdded(Contact)));
	connect(ContactManager::instance(), SIGNAL(contactAdded(Contact)),
			this, SLOT(contactAdded(Contact)));
	connect(ContactManager::instance(), SIGNAL(contactAboutToBeRemoved(Contact)),
			this, SLOT(contactAboutToBeRemoved(Contact)));
// 	connect(ContactManager::instance(), SIGNAL(contactRemoved(Contact)),
// 			this, SLOT(contactRemoved(Contact)));
	connect(ContactManager::instance(), SIGNAL(contactIdChanged(Contact, const QString &)),
			this, SLOT(contactIdChanged(Contact, const QString &)));

	connect(BuddyManager::instance(), SIGNAL(buddyUpdated(Buddy &)),
			this, SLOT(contactUpdated(Buddy &)));

	kdebugf2();
}

TlenProtocol::~TlenProtocol()
{
// 	disconnect(ContactManager::instance(), SIGNAL(contactAboutToBeAdded(Contact)),
// 			this, SLOT(contactAboutToBeAdded(Contact)));
	disconnect(ContactManager::instance(), SIGNAL(contactAdded(Contact)),
			this, SLOT(contactAdded(Contact)));
	disconnect(ContactManager::instance(), SIGNAL(contactAboutToBeRemoved(Contact)),
			this, SLOT(contactAboutToBeRemoved(Contact)));
// 	disconnect(ContactManager::instance(), SIGNAL(contactRemoved(Contact)),
// 			this, SLOT(contactRemoved(Contact)));
	disconnect(ContactManager::instance(), SIGNAL(contactIdChanged(Contact, const QString &)),
			this, SLOT(contactIdChanged(Contact, const QString &)));

	disconnect(BuddyManager::instance(), SIGNAL(buddyUpdated(Buddy &)),
			this, SLOT(contactUpdated(Buddy &)));

	logout();
}

void TlenProtocol::fetchAvatars(QString jid, QString type, QString md5)
{
	Q_UNUSED(type)
	Q_UNUSED(md5)

	kdebugf();

	Contact contact = ContactManager::instance()->byId(account(), jid, ActionCreateAndAdd);
	CurrentAvatarService->fetchAvatar(contact);

	kdebugf2();
}

void TlenProtocol::login(const QString &password, bool permanent)
{
	account().setPassword(password);
	account().setRememberPassword(permanent);
	account().setHasPassword(!password.isEmpty());

	connectToServer();
}

void TlenProtocol::connectToServer()
{
	kdebugf();

	if (TlenClient == 0)
	{
		TlenClient = new tlen;
		// odebranie wiadomosci
		connect( TlenClient, SIGNAL ( chatMsgReceived(QDomNode) ),
				this, SLOT (chatMsgReceived(QDomNode)) );
		// lista kontaktow - niedostepni
		connect( TlenClient, SIGNAL ( presenceDisconnected() ),
				this, SLOT (presenceDisconnected()) );
		// lista kontaktow - odebrano kontakt
		connect( TlenClient, SIGNAL ( itemReceived(QString, QString, QString, QString) ),
				this, SLOT (itemReceived(QString, QString, QString, QString)) );
		// lista kontaktow - zmiana statusu
		connect( TlenClient, SIGNAL ( presenceChanged(QString, QString, QString) ),
				this, SLOT (presenceChanged(QString, QString, QString)) );
		// autoryzacja kontaktu
		connect( this, SIGNAL ( authorize(QString,bool) ),
				TlenClient, SLOT (authorize(QString,bool)) );
		// pytanie o autoryzacje kontaktu
		connect( TlenClient, SIGNAL ( authorizationAsk(QString) ),
				this, SLOT (authorizationAsk(QString)) );
		// lista kontaktow - usuniecie kontaktu
		connect( TlenClient, SIGNAL ( removeItem(QString) ),
				this, SLOT (removeItem(QString)) );
		// lista kontaktow - sortuj roster
		connect( TlenClient, SIGNAL ( sortRoster() ),
				this, SLOT (sortRoster()) );
		// lista kontaktow - czysc liste kontaktow
		connect( TlenClient, SIGNAL ( clearRosterView() ),
				this, SLOT (clearRosterView()) );
		// tlen zalgowany
		connect( TlenClient, SIGNAL ( tlenLoggedIn() ),
				this, SLOT (tlenLoggedIn()) );
		// status zmieniony
		connect( TlenClient, SIGNAL ( statusChanged() ),
				this, SLOT (tlenStatusChanged()) );
		// status uaktualniony
		connect( TlenClient, SIGNAL ( statusUpdate() ),
				this, SLOT (tlenStatusUpdate()) );
		// odebranie wiadomosci
		connect( TlenClient, SIGNAL ( eventReceived(QDomNode) ),
				this, SLOT (eventReceived(QDomNode)) );
		// notyfikacja o pisaniu/alarmie
		connect( TlenClient, SIGNAL ( chatNotify(QString,QString) ),
				this, SLOT (chatNotify(QString,QString)) );
		// Pobranie avatara
		connect( TlenClient, SIGNAL (avatarReceived(QString,QString,QString) ),
				this, SLOT (fetchAvatars(QString,QString,QString)) );

	}

	TlenAccountDetails *tlenAccountDetails = dynamic_cast<TlenAccountDetails *>(account().details());
	if (!tlenAccountDetails)
		return;

	if (account().id().isEmpty())
	{
		MessageDialog::msg(tr("Tlen ID not set!"), false, "32x32/dialog-warning");
		setStatus(Status());
		kdebugmf(KDEBUG_FUNCTION_END, "end: Tlen id not set\n");
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

	if( TlenClient->isConnected() || TlenClient->isConnecting())
		TlenClient->closeConn();

	TlenClient->setReconnect(true);

	TlenClient->setUname(account().id());
	TlenClient->setPass (account().password());

	changeStatus(nextStatus());

	networkStateChanged(NetworkConnecting);
	kdebugf2();
}

void TlenProtocol::login()
{
	kdebugf();

	connectToServer();

	networkStateChanged(NetworkConnected);
	kdebugf2();
}

void TlenProtocol::logout()
{
	kdebugf();

	if (!TlenClient)
		return;

	if (TlenClient->isConnected() || TlenClient->isConnecting())
	{
		//TlenClient->setStatus("unavailable");
		TlenClient->closeConn();
		TlenClient->disconnect();
		setAllOffline();
	}

	delete TlenClient;
	TlenClient = 0;

	kdebugf2();
}

bool TlenProtocol::sendMessage(Chat chat, FormattedMessage &formattedMessage)
{
	kdebugf();

	kdebugm(KDEBUG_WARNING, "Tlen send count %d\n", chat.contacts().count());
	// TODO send to more users
	if (chat.contacts().count() != 1)
		return false;

	Contact contact = chat.contacts().toContact();
	QString tlenid = contact.id();
	QString plain = formattedMessage.toPlain();

	bool stop = false;
	//plain na QByteArray
	QByteArray data = unicode2cp(plain);
	emit filterOutgoingMessage(chat, data, stop);
	if (stop)
		return false;
	kdebugm(KDEBUG_WARNING, "Tlen send %s\n%s", qPrintable(tlenid), qPrintable(plain));
	TlenClient->writeMsg(plain, tlenid);

	HtmlDocument::escapeText(plain);

	Message message = Message::create();
	message.setMessageChat(chat);
	message.setType(Message::TypeSent);
	message.setMessageSender(account().accountContact());
	message.setContent(plain);
	message.setSendDate(QDateTime::currentDateTime());
	message.setReceiveDate(QDateTime::currentDateTime());

	emit messageSent(message);

	kdebugf2();
	return true;
}

void TlenProtocol::chatMsgReceived(QDomNode n)
{
	kdebugf();

	bool ignore = false;
	QDomElement msg = n.toElement();
	QString from = msg.attribute("from").split('/')[0]; // but what about res?
	QString fromresource = msg.attribute("from");
	QString body;
	QDateTime timeStamp;

	QDomNodeList nl = msg.childNodes();
	for (int i = 0; i < nl.count(); ++i)
	{
		QDomNode tmp=nl.item(i);
		if (tmp.nodeName() == "body")
			body=tmp.firstChild().toText().data();

		if (tmp.nodeName() == "x")
		{
			QDomElement e=tmp.toElement();
			if (e.hasAttribute("xmlns") && e.attribute("xmlns") == "jabber:x:delay")
			{
				timeStamp = QDateTime::fromString(e.attribute("stamp"), "yyyyMMdd'T'hh:mm:ss");
				timeStamp.setTimeSpec(Qt::UTC);
				//timeStamp=dt.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
			}
		}
	}

	if (timeStamp.isNull())
		timeStamp = QDateTime::currentDateTime();

	//		w->displayMsg(Tlen->decode(body.toUtf8()),timeStamp);

	Contact contact = ContactManager::instance()->byId(account(), TlenClient->decode(from), ActionCreateAndAdd);
	ContactSet contacts = ContactSet(contact);

	time_t msgtime = timeStamp.toTime_t();
	FormattedMessage formattedMessage(TlenClient->decode(body));

	QString plain = formattedMessage.toPlain();

	kdebugm(KDEBUG_WARNING, "Tlen message to %s\n%s", qPrintable(from), qPrintable(body));

	// TODO  : contacts?
	Chat chat = ChatManager::instance()->findChat(contacts);
	emit filterIncomingMessage(chat, contact, formattedMessage.toPlain(), msgtime, ignore);
	if (ignore)
		return;

	HtmlDocument::escapeText(plain);

	Message message = Message::create();
	message.setMessageChat(chat);
	message.setType(Message::TypeReceived);
	message.setMessageSender(contact);
	message.setContent(plain);
	message.setSendDate(timeStamp);
	message.setReceiveDate(QDateTime::currentDateTime());

	emit messageReceived(message);

	kdebugf2();
}

void TlenProtocol::presenceDisconnected()
{
	kdebugf();
	setAllOffline();
	networkStateChanged(NetworkDisconnected);
	kdebugf2();
}

Buddy TlenProtocol::nodeToBuddy(QDomNode node)
{
	Buddy result = Buddy::create();

	// TODO: 0.6.6 check if that contact is already in manager
	Contact contact = Contact::create();
	contact.setContactAccount(account());
	contact.setOwnerBuddy(result);
	contact.setId(account().id());

	TlenContactDetails *tlenDetails = new TlenContactDetails(contact);
	contact.setDetails(tlenDetails);

	QDomNodeList items = node.toElement().childNodes();
	for (int i=0;i<items.count();++i)
	{
		QDomElement mm = items.item(i).toElement();
		QString mmName = items.item(i).nodeName();
		if (mmName == "first")
		{
			result.setFirstName(mm.text());
		}
		else if (mmName == "last")
		{
			result.setLastName(mm.text());
		}
		else if (mmName == "nick")
		{
			result.setNickName(mm.text());
		}
		else if (mmName == "email")
		{
			result.setEmail(TlenClient->decode(mm.text()));
		}
		else if (mmName == "b")
		{
			result.setBirthYear(mm.text().toUShort());
		}
		else if (mmName == "s")
		{
			result.setGender((BuddyGender)mm.text().toUShort());
		}
		else if (mmName == "c")
		{
			result.setCity(mm.text());
		}
		else if (mmName == "r")
		{
			tlenDetails->setLookingFor(mm.text().toUShort());
		}
		else if (mmName == "j")
		{
			tlenDetails->setJob(mm.text().toUShort());
		}
		else if (mmName == "p")
		{
			tlenDetails->setTodayPlans(mm.text().toUShort());
		}
		else if (mmName == "v")
		{
			tlenDetails->setShowStatus(mm.text() == "1");
		}
		else if (mmName == "g")
		{
			tlenDetails->setHaveMic(mm.text() == "1");
		}
		else if (mmName == "k")
		{
			tlenDetails->setHaveCam(mm.text() == "1");
		}
	}

	//contact.setStatus();

	contact.setOwnerBuddy(result);
	return result;
}

void TlenProtocol::contactUpdated(Contact contact)
{
	if (contact.contactAccount() == account() || !isConnected() || !TlenClient)
		return;

	QStringList groupsList;
	//foreach (Group group, buddy.groups())
	//	groupsList.append(group.name());

	// TODO implement
	//JabberClient->updateContact(contact.id(), buddy.display(), QString());
}

void TlenProtocol::contactAdded(Contact contact)
{
	if (contact.contactAccount() != account() || !TlenClient)
		return;

	QStringList groupsList;

	//foreach (Group group, buddy.groups())
	//	groupsList.append(group.name());
	//TODO tlen pozwala na tylko 1 grupe
	//TODO opcja żądania autoryzacji, na razie na sztywno true

	TlenClient->addItem(contact.id(), contact.ownerBuddy().display(), QString(), true);
}

void TlenProtocol::contactAboutToBeRemoved(Contact contact)
{
	if (contact.contactAccount() != account() || !isConnected() || !TlenClient)
		return;

	TlenClient->remove(contact.id());
}


void TlenProtocol::contactIdChanged(Contact contact, const QString &oldId)
{
	Q_UNUSED(oldId)

	if (contact.contactAccount() != account() || !isConnected() || !TlenClient)
		return;

	// TODO
	//contactUpdated(buddy);
}

void TlenProtocol::itemReceived(QString jid, QString name, QString subscription, QString group)
{
	Q_UNUSED(subscription)

	kdebugf();
	kdebugm(KDEBUG_WARNING, "Tlen contact rcv %s\n", qPrintable(jid));

	Contact contact = ContactManager::instance()->byId(account(), jid, ActionCreateAndAdd);
	Buddy buddy = BuddyManager::instance()->byContact(contact, ActionCreateAndAdd);

	if (name.isEmpty())
	{/*buddy.setDisplay(jid);*/} // BM sets display
	else
		buddy.setDisplay(name);

	if (!group.isEmpty())
		buddy.addToGroup(GroupManager::instance()->byName(group, true /* create group */));

	buddy.setAnonymous(false);

	// TODO: 0.6.6 remember to set every contact offline after add to contact list
	//presenceChanged(jid, "unavailable", QString());

	kdebugf2();
}

void TlenProtocol::presenceChanged(QString from, QString newstatus, QString description)
{
	kdebugf();

	Status status;
	if(newstatus == "away")
		status.setType("Away");
	else if(newstatus == "xa")
		status.setType("NotAvailable");
	else if(newstatus == "dnd")
		status.setType("DoNotDisturb");
	else if(newstatus == "available")
		status.setType("Online");
	else if(newstatus == "chat")
		status.setType("FreeForChat");
	else if(newstatus == "invisible")
		status.setType("Invisible");
	else if(newstatus == "unavailable")
		status.setType("Offline");
	else
		status.setType("Offline");

	if (!description.isEmpty())
		status.setDescription(description);

	QString jid(from.split('/')[0]); // to dziala

	// find user@server





	kdebugm(KDEBUG_WARNING, "Tlen status change: %s %s\n%s", qPrintable(from), qPrintable(newstatus), qPrintable(description));


	//if (contact.isNull())
	//	return;

	// find id

	Contact contact = ContactManager::instance()->byId(account(), jid, ActionCreateAndAdd);
	Buddy buddy = BuddyManager::instance()->byContact(contact, ActionCreateAndAdd);

	if (buddy.isAnonymous())
		buddy.setAnonymous(false);

	// id resource add new contact
	if (jid != from)
	{
		Contact contactRes = ContactManager::instance()->byId(account(), from, ActionCreateAndAdd);
		contactRes.setOwnerBuddy(buddy);

		Status oldStatus = contactRes.currentStatus();
		contactRes.setCurrentStatus(status);

		if (!TypingUsers[from].isEmpty())
			TypingUsers[from] = description;

		emit contactStatusChanged(contactRes, oldStatus);

		// if general jid status was same so need to set same also or calc some
		if (contact.currentStatus() == oldStatus)
		{
			contact.setCurrentStatus(status);
			emit contactStatusChanged(contact, oldStatus);
		}
		return;
	}

	/* is this contact really anonymous? - need deep check
	if (contact.isAnonymous())
	{
		// TODO - ignore! - przynajmniej na razie
		// emit userStatusChangeIgnored(contact);
		// userlist->addUser(contact);
		return;
	}
	*/

	Status oldStatus = contact.currentStatus();
	contact.setCurrentStatus(status);

	if (!TypingUsers[from].isEmpty())
		TypingUsers[from] = description;

	emit contactStatusChanged(contact, oldStatus);
	kdebugf2();
}

void TlenProtocol::authorizationAsk(QString to)
{
	kdebugf();
//	if( rosterItem *item = rosterModel->find(from) ) {
//		if( item->data(3)=="subscribed" || item->data(3)=="both" )
//			return;
//	}

	if(QMessageBox::question(0, //this,
				tr("User authorization"),
				tr("User %1 requested authorization from You."
				"Do you want to authorize him?")
				.arg(to),
				tr("&Yes"), tr("&No"),
				QString(), 2, 1))
		emit authorize(to, false);
	else
		emit authorize(to, true);
}

void TlenProtocol::removeItem(QString a)
{
	Q_UNUSED(a)

	kdebugf();
}

void TlenProtocol::sortRoster()
{
	kdebugf();
}

void TlenProtocol::clearRosterView()
{
	kdebugf();
}

void TlenProtocol::tlenLoggedIn()
{
	kdebugf();
}

void TlenProtocol::tlenStatusChanged() // FOR GUI TO UPDATE ICONS
{
	kdebugf();
}

void TlenProtocol::tlenStatusUpdate()	// FOR TLEN TO WRITE STATUS
{
	kdebugf();
}

void TlenProtocol::eventReceived(QDomNode a)
{
	Q_UNUSED(a)

	kdebugf();
}

void TlenProtocol::chatNotify(QString from, QString type)
{
	kdebugf();

	Contact contact = ContactManager::instance()->byId(account(), from);
	if (contact.isNull())
		return;

	Status oldStatus = contact.currentStatus();
	Status newStatus = contact.currentStatus();

	if (type=="t")
	{
		if (TypingUsers.contains(from))
			return;

		//typing start
		QString oldDesc = oldStatus.description();
		TypingUsers.insert(from, oldDesc);
		newStatus.setDescription(QString("[pisze] %1").arg(oldDesc));
		contact.setCurrentStatus(newStatus);
		emit contactStatusChanged(contact, oldStatus);
	}
	else if (type=="u")
	{
		//typing stop
		QString oldDesc = TypingUsers[from];
		TypingUsers.remove(from);
		newStatus.setDescription(oldDesc);
		contact.setCurrentStatus(newStatus);
		emit contactStatusChanged(contact, oldStatus);
	}
	else if(type=="a")
	{
		//alarm
	}
}

bool TlenProtocol::validateUserID(const QString& uid)
{
	Q_UNUSED(uid)

	//TODO:
	return true;
}

void TlenProtocol::changeStatus(Status status)
{
	const QString &type = status.type();

	// TODO: do sf if not connected
	if (TlenClient == 0 && !nextStatus().isDisconnected())
		login();

	if("Online" == type)
		TlenClient->setStatusDescr(tlen::available, status.description());
	else if("FreeForChat" == type)
		TlenClient->setStatusDescr(tlen::chat, status.description());
	else if("DoNotDisturb" == type)
		TlenClient->setStatusDescr(tlen::dnd, status.description());
	else if("Away" == type)
		TlenClient->setStatusDescr(tlen::away, status.description());
	else if("NotAvailable" == type)
		TlenClient->setStatusDescr(tlen::xa, status.description());
	else if("Invisible" == type)
		TlenClient->setStatusDescr(tlen::invisible, status.description());
	else
	{
		// Offline
		TlenClient->setStatusDescr(tlen::unavailable, status.description());

		networkStateChanged(NetworkDisconnected);

		setAllOffline();

		if (!nextStatus().isDisconnected())
			setStatus(Status());
	}

	Protocol::statusChanged(status);
}

void TlenProtocol::changeStatus()
{
	Status newStatus = nextStatus();

	if (newStatus.isDisconnected() && status().isDisconnected())
	{
		logout();
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

void TlenProtocol::changePrivateMode()
{
	changeStatus();
}

QPixmap TlenProtocol::statusPixmap(Status status)
{
	return StatusTypeManager::instance()->statusPixmap("tlen", "16x16", status.type(),
			!status.description().isEmpty(), false);
}

QPixmap TlenProtocol::statusPixmap(const QString &statusType)
{
	return StatusTypeManager::instance()->statusPixmap("tlen", "16x16", statusType, false, false);
}

QIcon TlenProtocol::statusIcon(Status status)
{
	return StatusTypeManager::instance()->statusIcon("tlen", status.type(),
			!status.description().isEmpty(), false);
}

QIcon TlenProtocol::statusIcon(const QString &statusType)
{
	return StatusTypeManager::instance()->statusIcon("tlen", statusType, false, false);
}

