/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <stdio.h>
#include <qcheckbox.h>
#include <qtimer.h>
#include <iostream>
#include <qmessagebox.h>
#include <qregexp.h>
#include <QHostInfo>
#include <QtGui/QPushButton>
#include <QtGui/QMessageBox>

#include "accounts/account.h"
#include "accounts/account-manager.h"

#include "buddies/buddy-manager.h"

#include "configuration/configuration-file.h"
#include "core/core.h"

#include "gui/windows/kadu-window.h"
#include "gui/windows/message-dialog.h"
#include "gui/windows/password-window.h"
#include "gui/windows/main-configuration-window.h"

#include "chat/message/message.h"
#include "debug.h"
#include "icons-manager.h"
#include "status/status.h"
#include "misc/misc.h"
#include "configuration/xml-configuration-file.h"
#include "html_document.h"

#include "tlen.h"

#include "tlen-account-details.h"
#include "tlen-contact-details.h"
#include "tlen-protocol-factory.h"

#include "tlen-protocol.h"

#include "exports.h"

extern "C" KADU_EXPORT int tlen_protocol_init()
{
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

	ProtocolsManager::instance()->registerProtocolFactory(TlenProtocolFactory::instance());

	kdebugf2();
	return 0;
}

void TlenProtocol::closeModule()
{
	kdebugf();

	ProtocolsManager::instance()->unregisterProtocolFactory(TlenProtocolFactory::instance());

	kdebugf2();
}

TlenProtocol::TlenProtocol(Account account, ProtocolFactory *factory): Protocol(account,factory), TlenClient(0)
{
	kdebugf();

	CurrentChatService = new TlenChatService(this);
	CurrentAvatarService = new TlenAvatarService(this);
	CurrentPersonalInfoService = new TlenPersonalInfoService(this);

	connect(BuddyManager::instance(), SIGNAL(buddyAdded(Buddy &)),
			this, SLOT(contactAdded(Buddy &)));
	connect(BuddyManager::instance(), SIGNAL(buddyRemoved(Buddy &)),
			this, SLOT(contactRemoved(Buddy &)));
	connect(BuddyManager::instance(), SIGNAL(contactAdded(Buddy &, Account)),
			this, SLOT(contactAboutToBeRemoved(Buddy & , Account)));
	connect(BuddyManager::instance(), SIGNAL(contactAboutToBeRemoved(Buddy &, Account)),
			this, SLOT(contactAboutToBeRemoved(Buddy &, Account)));
	connect(BuddyManager::instance(), SIGNAL(buddyUpdated(Buddy &)),
			this, SLOT(contactUpdated(Buddy &)));
	connect(BuddyManager::instance(), SIGNAL(contactIdChanged(Buddy &, Account, const QString &)),
			this, SLOT(contactAccountIdChanged(Buddy &, Account, const QString &)));

	kdebugf2();
}

TlenProtocol::~TlenProtocol()
{
	disconnect(BuddyManager::instance(), SIGNAL(buddyAdded(Buddy &)),
			this, SLOT(contactAdded(Buddy &)));
	disconnect(BuddyManager::instance(), SIGNAL(buddyRemoved(Buddy &)),
			this, SLOT(contactRemoved(Buddy &)));
	disconnect(BuddyManager::instance(), SIGNAL(contactAdded(Buddy &, Account)),
			this, SLOT(contactAboutToBeRemoved(Buddy & , Account)));
	disconnect(BuddyManager::instance(), SIGNAL(contactAboutToBeRemoved(Buddy &, Account)),
			this, SLOT(contactAboutToBeRemoved(Buddy &, Account)));
	disconnect(BuddyManager::instance(), SIGNAL(buddyUpdated(Buddy &)),
			this, SLOT(contactUpdated(Buddy &)));
	disconnect(BuddyManager::instance(), SIGNAL(contactIdChanged(Buddy &, Account, const QString &)),
			this, SLOT(contactAccountIdChanged(Buddy &, Account, const QString &)));

	logout();
}

void TlenProtocol::fetchAvatars(QString jid, QString type, QString md5)
{
	kdebugf();

	Buddy buddy = account().getBuddyById(jid);

 	if (buddy.isAnonymous())
		buddy.setAnonymous(false);

	CurrentAvatarService->fetchAvatar(buddy.contact(account()));

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
		connect( TlenClient, SIGNAL ( itemReceived(QString, QString, QString, QString,bool) ),
				this, SLOT (itemReceived(QString, QString, QString, QString,bool)) );
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
		MessageDialog::msg(tr("Tlen ID not set!"), false, "Warning");
		setStatus(Status());
		kdebugmf(KDEBUG_FUNCTION_END, "end: Tlen id not set\n");
		return;
	}

	if (!account().hasPassword())
	{
		PasswordWindow::getPassword(tr("Please provide password for %1 account").arg(account().name()),
				this, SLOT(login(const QString &, bool)));
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

bool TlenProtocol::sendMessage(Chat *chat, FormattedMessage &formattedMessage)
{
	kdebugf();

	BuddySet users = chat->buddies();
	// TODO send to more users
	Buddy buddy = (*users.begin());
	QString plain = formattedMessage.toPlain();
	QString tlenid = buddy.id(account());

	bool stop = false;
	//plain na QByteArray
	QByteArray data = unicode2cp(plain);
	emit sendMessageFiltering(chat, data, stop);
	if (stop)
		return false;
	kdebugm(KDEBUG_WARNING, "Tlen send %s\n%s", qPrintable(tlenid), qPrintable(plain));
	TlenClient->writeMsg(plain, tlenid);

	HtmlDocument::escapeText(plain);

	Message message(chat, Message::TypeSent, Core::instance()->myself());
	message
		.setContent(plain)
		.setSendDate(QDateTime::currentDateTime())
		.setReceiveDate(QDateTime::currentDateTime());

	emit messageSent(message);

	kdebugf2();
	return true;
}

void TlenProtocol::chatMsgReceived(QDomNode n)
{
	kdebugf();
	bool ignore = false;
	QDomElement msg = n.toElement();
	QString from = msg.attribute("from");
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

	// TODO - zaimplementowac to samo w ContactList
	Buddy buddy = account().getBuddyById(from);
	BuddySet contacts = BuddySet(buddy);
	// FIXME: dunno why, but commenting it fixed for now (08.04.2009) problem with finding chat for contact (conference window was always being opened for 1 contact)
	//contacts << contact;

	time_t msgtime = timeStamp.toTime_t();
	FormattedMessage formattedMessage(TlenClient->decode(body));

	QString plain = formattedMessage.toPlain();

	kdebugm(KDEBUG_WARNING, "Tlen message to %s\n%s", qPrintable(from), qPrintable(body));

	// TODO  : contacts?
	Chat *chat = this->findChat(contacts);
	emit receivedMessageFilter(chat, buddy, formattedMessage.toPlain(), msgtime, ignore);
	if (ignore)
		return;

	HtmlDocument::escapeText(plain);

	Message message(chat, Message::TypeReceived, buddy);
	message
		.setContent(plain)
		.setSendDate(timeStamp)
		.setReceiveDate(QDateTime::currentDateTime());

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
	Buddy result;

	Contact contact;
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
			result.setEmail(mm.text());
		}
		else if (mmName == "b")
		{
			result.setBirthYear(mm.text().toUShort());
		}
		else if (mmName == "s")
		{
			result.setGender((BuddyShared::BuddyGender)mm.text().toUShort());
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

	//result.setStatus();

	result.addContact(contact);
	return result;
}

void TlenProtocol::contactAdded(Buddy &buddy)
{
	Contact contact = buddy.contact(account());
	if (contact.isNull() || buddy.isAnonymous() || !TlenClient)
		return;

	QStringList groupsList;

	//foreach (Group group, buddy.groups())
	//	groupsList.append(group.name());
	//TODO tlen pozwala na tylko 1 grupe
	//TODO opcja żądania autoryzacji, na razie na sztywno true

	TlenClient->addItem(contact.id(), buddy.display(), QString(), true);
}

void TlenProtocol::contactRemoved(Buddy &buddy)
{
	Contact contact = buddy.contact(account());
	if (contact.isNull() || !isConnected() || !TlenClient)
		return;

	TlenClient->remove(contact.id());
}

void TlenProtocol::contactUpdated(Buddy &buddy)
{
	Contact contact = buddy.contact(account());
	if (contact.isNull() || buddy.isAnonymous() || !TlenClient)
		return;

	QStringList groupsList;
	//foreach (Group group, buddy.groups())
	//	groupsList.append(group.name());

	// TODO implement
	//JabberClient->updateContact(contact.id(), buddy.display(), QString());
}

void TlenProtocol::contactAdded(Buddy &buddy, Account contactAccount)
{
	if (contactAccount != account())
		return;

	contactAdded(buddy);
}

void TlenProtocol::contactAboutToBeRemoved(Buddy &buddy, Account contactAccount)
{
	if (contactAccount != account())
		return;

	contactRemoved(buddy);
}


void TlenProtocol::contactAccountIdChanged(Buddy &buddy, Account contactAccount, const QString &oldId)
{
	if (contactAccount != account())
		return;

	contactUpdated(buddy);
}

void TlenProtocol::itemReceived(QString jid, QString name, QString subscription, QString group, bool sort)
{
	kdebugf();
	kdebugm(KDEBUG_WARNING, "Tlen contact rcv %s\n", qPrintable(jid));

	Buddy buddy = BuddyManager::instance()->byId(account(), jid);

	if(!name.isNull())
		buddy.setDisplay(name);

 	if (buddy.isAnonymous())
		buddy.setAnonymous(false);

	// remember to set every contact offline after add to contact list
	presenceChanged(jid, "unavailable", QString::null);

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

	Buddy buddy = BuddyManager::instance()->byId(account(), from);

	kdebugm(KDEBUG_WARNING, "Tlen status change: %s %s\n%s", qPrintable(from), qPrintable(newstatus), qPrintable(description));

	if (buddy.isAnonymous())
		buddy.setAnonymous(false);

	/* is this contact realy anonymous? - need deep check
	if (contact.isAnonymous())
	{
		// TODO - ignore! - przynajmniej na razie
		// emit userStatusChangeIgnored(contact);
		// userlist->addUser(contact);
		return;
	}
	*/

	Contact contact = buddy.contact(account());
	if (contact.isNull())
		return;

	Status oldStatus = contact.currentStatus();
	contact.setCurrentStatus(status);

	if (!TypingUsers[from].isEmpty())
		TypingUsers[from] = description;

	emit buddyStatusChanged(account(), buddy, oldStatus);
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
	kdebugf();
}

void TlenProtocol::chatNotify(QString from, QString type)
{
	kdebugf();

	Buddy buddy = BuddyManager::instance()->byId(account(), from);

	Contact contact = buddy.contact(account());
	if (contact.isNull())
		return;

	Status oldStatus = contact.currentStatus();
	Status newStatus = contact.currentStatus();

	if(type=="t")
	{
		if (TypingUsers.contains(from))
			return;

		//typing start
		QString oldDesc = oldStatus.description();
		TypingUsers.insert(from, oldDesc);
		newStatus.setDescription(QString("[pisze] %1").arg(oldDesc));
		contact.setCurrentStatus(newStatus);
		emit buddyStatusChanged(account(), buddy, oldStatus);
	}
	else if(type=="u")
	{
		//typing stop
		QString oldDesc = TypingUsers[from];
		TypingUsers.remove(from);
		newStatus.setDescription(oldDesc);
		contact.setCurrentStatus(newStatus);
		emit buddyStatusChanged(account(),buddy, oldStatus);
	}
	else if(type=="a")
	{
		//alarm
	}
}

bool TlenProtocol::validateUserID(QString& uid)
{
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
		TlenClient->setStatusDescr("available", status.description());
	else if("FreeForChat" == type)
		TlenClient->setStatusDescr("chat", status.description());
	else if("DoNotDisturb" == type)
		TlenClient->setStatusDescr("dnd", status.description());
	else if("Away" == type)
		TlenClient->setStatusDescr("away", status.description());
	else if("NotAvailable" == type)
		TlenClient->setStatusDescr("xa", status.description());
	else if("Invisible" == type)
		TlenClient->setStatusDescr("invisible", status.description());
	else
	{
		// Offline
		TlenClient->setStatusDescr("unavailable", status.description());

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
	QString pixmapName(dataPath("kadu/modules/data/tlen_protocol/"));

	QString groupName = status.type();

	if ("Online" == groupName)
		pixmapName.append("online");

	else if ("FreeForChat" == groupName)
		pixmapName.append("chat");

	else if ("DoNotDisturb" == groupName)
		pixmapName.append("dnd");

	else if ("Away" == groupName)
		pixmapName.append("away");

	else if ("NotAvailable" == groupName)
		pixmapName.append("unavailable");

	else if ("Invisible" == groupName)
		pixmapName.append("invisible");

	else	pixmapName.append("offline");

	pixmapName.append(status.description().isNull()
			? ".png"
			: "i.png");

	return IconsManager::instance()->loadPixmap(pixmapName);
}

QPixmap TlenProtocol::statusPixmap(const QString &statusType)
{
	QString pixmapName(dataPath("kadu/modules/data/tlen_protocol/"));

	if ("Online" == statusType)
		pixmapName.append("online");

	else if ("FreeForChat" == statusType)
		pixmapName.append("chat");

	else if ("DoNotDisturb" == statusType)
		pixmapName.append("dnd");

	else if ("Away" == statusType)
		pixmapName.append("away");

	else if ("NotAvailable" == statusType)
		pixmapName.append("unavailable");

	else if ("Invisible" == statusType)
		pixmapName.append("invisible");

	else	pixmapName.append("offline");

	pixmapName.append(".png");

	return IconsManager::instance()->loadPixmap(pixmapName);
}
