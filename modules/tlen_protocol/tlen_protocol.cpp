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

#include "action.h"
#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "chat/message/message.h"
#include "config_file.h"
#include "contacts/contact-manager.h"
#include "core/core.h"
#include "gui/windows/kadu-window.h"
#include "debug.h"
//#include "chat/chat_manager-old.h"
#include "icons-manager.h"
#include "main_configuration_window.h"
#include "protocols/status.h"
#include "misc/misc.h"
#include "message_box.h"
#include "xml_config_file.h"

#include "tlen.h"

#include "tlen-contact-account-data.h"
#include "tlen_account.h"
#include "tlen_protocol_factory.h"
#include "tlen_protocol.h"

extern "C" int tlen_protocol_init()
{
	return TlenProtocol::initModule();
}

extern "C" void tlen_protocol_close()
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

TlenProtocol::TlenProtocol(Account *account, ProtocolFactory *factory): Protocol(account,factory), TlenClient(0)
{
	kdebugf();

	CurrentChatService = new TlenChatService(this);

	kdebugf2();
}

TlenProtocol::~TlenProtocol()
{
	logout();
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
		//
		connect( TlenClient, SIGNAL ( chatNotify(QString,QString) ),
				this, SLOT (chatNotify(QString,QString)) );
	}

	TlenAccount *tlenAccount = dynamic_cast<TlenAccount *>(account());
	if (tlenAccount->id().isNull() || tlenAccount->password().isNull())
	{
		MessageBox::msg(tr("tlen ID or password not set!"), false, "Warning");
		//NextStatus->setOffline();
		kdebugmf(KDEBUG_FUNCTION_END, "end: Tlen ID or password not set\n");
		return;
	}

	if( TlenClient->isConnected() )
		TlenClient->closeConn();

	TlenClient->setReconnect(true);

	TlenClient->setUname(tlenAccount->id());
	TlenClient->setPass (tlenAccount->password());

	changeStatus(nextStatus());

	networkStateChanged(NetworkConnecting);
	kdebugf2();
}

void TlenProtocol::login()
{
	kdebugf();

	connectToServer();
	setStatus(Status::Online);
	networkStateChanged(NetworkConnected);
	kdebugf2();
}

void TlenProtocol::logout()
{
	kdebugf();

	if (TlenClient != 0 )
	{
		if (TlenClient->isConnected())
		{
			TlenClient->setStatus("unavailable");
			TlenClient->closeConn();
			TlenClient->disconnect();
			setAllOffline();
		}

		delete TlenClient;
		TlenClient = 0;
	}

	kdebugf2();
}

bool TlenProtocol::sendMessage(Chat *chat, FormattedMessage &formattedMessage)
{
	kdebugf();

	ContactSet users = chat->contacts();
	// TODO send to more users
	Contact contact = (*users.begin());
	QString plain = formattedMessage.toPlain();
	QString tlenid = contact.id(account());

	bool stop = false;
	//plain na QByteArray
	QByteArray data = unicode2cp(plain);
	emit sendMessageFiltering(chat, data, stop);
	if (stop)
		return false;
	kdebugm(KDEBUG_WARNING, "Tlen send %s\n%s", qPrintable(tlenid), qPrintable(plain));
	TlenClient->writeMsg(plain,tlenid);

	Message message;
	message.chat = chat;
	message.messageContent = formattedMessage.toPlain();
	message.sender = Core::instance()->myself();
	message.sendDate = QDateTime::currentDateTime();
	message.receiveDate = QDateTime();
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
	Contact contact = account()->getContactById(from);
	ContactSet contacts = ContactSet(contact);
	// FIXME: dunno why, but commenting it fixed for now (08.04.2009) problem with finding chat for contact (conference window was always being opened for 1 contact)
	//contacts << contact;

	time_t msgtime = timeStamp.toTime_t();
	FormattedMessage formattedMessage(TlenClient->decode(body));

	kdebugm(KDEBUG_WARNING, "Tlen message to %s\n%s", qPrintable(from), qPrintable(body));

	// TODO  : contacts?
	Chat *chat = this->findChat(contacts);
	emit receivedMessageFilter(chat, contact, formattedMessage.toPlain(), msgtime, ignore);
	if (ignore)
		return;

	Message message;
	message.chat = chat;
	message.messageContent = formattedMessage.toPlain();
	message.sender = contact;
	message.sendDate = timeStamp;
	message.receiveDate = QDateTime::currentDateTime();
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

void TlenProtocol::itemReceived(QString jid, QString name, QString subscription, QString group, bool sort)
{
	kdebugf();
	kdebugm(KDEBUG_WARNING, "Tlen contact rcv %s\n", qPrintable(jid));

	Contact contact = account()->getContactById(jid);

	if(!name.isNull())
		contact.setDisplay(name);

 	if (contact.isAnonymous())
	{
		ContactManager::instance()->addContact(contact);
	}

	// remember to set every contact offline after add to cntact list
	presenceChanged(jid, "unavailable", QString::null);

	kdebugf2();
}

void TlenProtocol::presenceChanged(QString from, QString newstatus, QString description)
{
	kdebugf();

	Status status;
	if(newstatus == "away")
		status.setType(Status::Busy);
	else if(newstatus == "xa")
		status.setType(Status::Busy);
	else if(newstatus == "dnd")
		status.setType(Status::Busy);
	else if(newstatus == "available")
		status.setType(Status::Online);
	else if(newstatus == "chat")
		status.setType(Status::Online);
	else if(newstatus == "invisible")
		status.setType(Status::Invisible);
	else if(newstatus == "unavailable")
		status.setType(Status::Offline);
	else
		status.setType(Status::Offline);

	if (!description.isEmpty())
		status.setDescription(description);

	Contact contact = account()->getContactById(from);

	kdebugm(KDEBUG_WARNING, "Tlen status change: %s %s\n%s", qPrintable(from), qPrintable(newstatus), qPrintable(description));

	/* is this contact realy anonymous? - need deep check
	if (contact.isAnonymous())
	{
		// TODO - ignore! - przynajmniej na razie
		// emit userStatusChangeIgnored(contact);
		// userlist->addUser(contact);
		return;
	}
	*/

	TlenContactAccountData *data = dynamic_cast<TlenContactAccountData *>(contact.accountData(account()));

	if (!data)
		return;

	Status oldStatus = data->status();
	data->setStatus(status);

	if (!TypingUsers[from].isEmpty())
		TypingUsers[from] = description;

	emit contactStatusChanged(account(), contact, oldStatus);
	kdebugf2();
}

void TlenProtocol::authorizationAsk(QString from)
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
				.arg(from),
				tr("&Yes"), tr("&No"),
				QString(), 2, 1))
		emit authorize(from, false);
	else
		emit authorize(from, true);
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

	Contact contact = account()->getContactById(from);

	TlenContactAccountData *data = dynamic_cast<TlenContactAccountData *>(contact.accountData(account()));

	if (!data)
		return;

	Status oldStatus = data->status();
	Status newStatus = data->status();

	if(type=="t")
	{
		//typing start
		QString oldDesc = oldStatus.description();
		TypingUsers.insert(from, oldDesc);
		newStatus.setDescription(QString("[pisze] %1").arg(oldDesc));
		data->setStatus(newStatus);
		emit contactStatusChanged(account(), contact, oldStatus);
	}
	else if(type=="u")
	{
		//typing stop
		QString oldDesc = TypingUsers[from];
		TypingUsers.remove(from);
		newStatus.setDescription(oldDesc);
		data->setStatus(newStatus);
		emit contactStatusChanged(account(), contact, oldStatus);
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
	// TODO: do sf if not connected
	if (TlenClient == 0)
		return;

	// TODO: add rest status options
	if(status.isOnline())
		TlenClient->setStatusDescr("available", status.description());
	else if(status.isInvisible())
		TlenClient->setStatusDescr("invisible", status.description());
	else if(status.isBusy())
		TlenClient->setStatusDescr("away", status.description());
	else
		TlenClient->setStatusDescr("unavailable", status.description());

	if (status.isOffline())
	{
		networkStateChanged(NetworkDisconnected);

		setAllOffline();

		if (!nextStatus().isOffline())
			setStatus(Status::Offline);
	}

	Protocol::statusChanged(status);
}

void TlenProtocol::changeStatus()
{
	Status newStatus = nextStatus();

	if (newStatus.isOffline() && status().isOffline())
	{
		networkStateChanged(NetworkDisconnected);

		setAllOffline();

		if (!nextStatus().isOffline())
			setStatus(Status::Offline);

		return;
	}

	if (NetworkConnecting == state())
		return;

	if (status().isOffline())
	{
		login();
		return;
	}

	statusChanged(newStatus);
}

void TlenProtocol::changePrivateMode()
{
	changeStatus();
}

QPixmap TlenProtocol::statusPixmap(Status status)
{
	QString pixmapName(dataPath("kadu/modules/data/tlen_protocol/"));

	switch (status.type())
	{
		case Status::Online:
			pixmapName.append("online");
			break;
		case Status::Busy:
			pixmapName.append("away");
			break;
		case Status::Invisible:
			pixmapName.append("invisible");
			break;
		default:
			pixmapName.append("offline");
			break;
	}

	pixmapName.append(status.description().isNull()
			? ".png"
			: "i.png");

	return IconsManager::instance()->loadPixmap(pixmapName);
}
