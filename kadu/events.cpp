/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qwidget.h>
#include <qdialog.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qstring.h>
#include <qfile.h>
#include <qlayout.h>
#include <qmessagebox.h>

#include <netinet/in.h>
#include <errno.h>

#include "kadu.h"
#include "ignore.h"
#include "events.h"
#include "chat.h"
#include "history.h"
#include "pending_msgs.h"
#include "dock_widget.h"
#include "debug.h"
#include "sound.h"
#include "dcc.h"
#include "config_file.h"
#include "../config.h"
#ifdef HAVE_OPENSSL
extern "C"
{
#include "simlite.h"
};
#endif

AutoConnectionTimer *AutoConnectionTimer::autoconnection_object = NULL;

QTime lastsoundtime;

AutoConnectionTimer::AutoConnectionTimer(QObject *parent) : QTimer(parent, "AutoConnectionTimer") {
	connect(this, SIGNAL(timeout()), SLOT(doConnect()));
	start(1000, TRUE);
}

void AutoConnectionTimer::doConnect() {
	kadu->setStatus(loginparams.status & (~GG_STATUS_FRIENDS_MASK));	
}

void AutoConnectionTimer::on() {
	if (!autoconnection_object)
		autoconnection_object = new AutoConnectionTimer();
}

void AutoConnectionTimer::off() {
	if (autoconnection_object) {
		delete autoconnection_object;
		autoconnection_object = NULL;
		}
}

SavePublicKey::SavePublicKey(uin_t uin, QString keyData, QWidget *parent, const char *name) :
	QDialog(parent, name, Qt::WDestructiveClose), uin(uin), keyData(keyData) {
	
	kdebug("SavePublicKey::SavePublicKey()\n");

	setCaption(i18n("Save public key"));
	resize(200, 80);
	
	QLabel *l_info = new QLabel(
		i18n("User %1 is sending you his public key. Do you want to save it?").arg(userlist.byUin(uin).altnick),
		this);

	QPushButton *yesbtn = new QPushButton(i18n("Yes"), this);
	QPushButton *nobtn = new QPushButton(i18n("No"), this);

	QObject::connect(yesbtn, SIGNAL(clicked()), this, SLOT(yesClicked()));
	QObject::connect(nobtn, SIGNAL(clicked()), this, SLOT(reject()));

	QGridLayout *grid = new QGridLayout(this, 2, 2, 3, 3);
	grid->addMultiCellWidget(l_info, 0, 0, 0, 1);
	grid->addWidget(yesbtn, 1, 0);
	grid->addWidget(nobtn, 1, 1);

	kdebug("SavePublicKey::SavePublicKey(): finished\n");
}

void SavePublicKey::yesClicked() {
	QFile keyfile;
	QString keyfile_path;

	kdebug("SavePublicKey::yesClicked()\n");

	keyfile_path.append(ggPath("keys/"));
	keyfile_path.append(QString::number(uin));
	keyfile_path.append(".pem");

	keyfile.setName(keyfile_path);

	if (!(keyfile.open(IO_WriteOnly))) {
		QMessageBox::critical(this, i18n("Error"), i18n("Error writting the key"), i18n("OK"), QString::null, 0);
		kdebug("eventRecvMsg(): Error opening key file %s\n", (const char *)keyfile_path.local8Bit());
		return;
		}
	else {
		keyfile.writeBlock(keyData.local8Bit(), keyData.length());
		keyfile.close();
		int i = 0;
		UinsList uins;
		uins.append(uin);
		while (i < chats.count() && !chats[i].uins.equals(uins))
			i++;
		if (i < chats.count())
			chats[i].ptr->setEncryptionBtnEnabled(true);
		}
	accept();

	kdebug("SavePublicKey::yesClicked(): finished\n");
}

EventManager::EventManager()
{
	connect(this,SIGNAL(connected()),this,SLOT(connectedSlot()));
	connect(this,SIGNAL(connectionFailed()),this,SLOT(connectionFailedSlot()));
	connect(this,SIGNAL(connectionBroken()),this,SLOT(connectionBrokenSlot()));
	connect(this,SIGNAL(disconnected()),this,SLOT(disconnectedSlot()));
	connect(this,SIGNAL(userStatusChanged(struct gg_event*)),this,SLOT(userStatusChangedSlot(struct gg_event*)));
	connect(this,SIGNAL(userlistReceived(struct gg_event*)),this,SLOT(userlistReceivedSlot(struct gg_event*)));
	connect(this,SIGNAL(messageReceived(int,UinsList,unsigned char*,time_t, int, void *)),this,SLOT(messageReceivedSlot(int,UinsList,unsigned char*,time_t, int, void *)));
	connect(this,SIGNAL(chatReceived(UinsList,const QString&,time_t)),
		this,SLOT(chatReceivedSlot(UinsList,const QString&,time_t)));
	connect(this,SIGNAL(ackReceived(int)),this,SLOT(ackReceivedSlot(int)));
	connect(this,SIGNAL(dccConnectionReceived(const UserListElement&)),
		this,SLOT(dccConnectionReceivedSlot(const UserListElement&)));
	connect(this,SIGNAL(pubdirReplyReceived(gg_pubdir50_t)),
		this,SLOT(pubdirReplyReceivedSlot(gg_pubdir50_t)));
	connect(this,SIGNAL(userlistReplyReceived(char, char *)),
		this,SLOT(userlistReplyReceivedSlot(char, char *)));
};

void EventManager::connectedSlot()
{
	kadu->doBlink = false;
	sendUserlist();
	kadu->setCurrentStatus(loginparams.status & (~GG_STATUS_FRIENDS_MASK));
	userlist_sent = true;
	if (ifStatusWithDescription(loginparams.status))
		kadu->setStatus(loginparams.status & (~GG_STATUS_FRIENDS_MASK));
	/* uruchamiamy autoawaya(jezeli wlaczony) po wyslaniu userlisty i ustawieniu statusu */
	if (config_file.readBoolEntry("Global","AutoAway"))
		AutoAwayTimer::on();
	/* jezeli sie rozlaczymy albo stracimy polaczenie, proces laczenia sie z serwerami zaczyna sie od poczatku */
	server_nr = 0;
	pingtimer = new QTimer;
	QObject::connect(pingtimer, SIGNAL(timeout()), kadu, SLOT(pingNetwork()));
	pingtimer->start(60000, TRUE);
		
/* Zauwazy³em ze to wogóle nic nie robi... wiêc co zakomentowa³em..
	readevent = new QTimer;
	QObject::connect(readevent, SIGNAL(timeout()), kadu, SLOT(checkConnection()));    
	readevent->start(10000, TRUE);
*/
};

void EventManager::connectionFailedSlot()
{
	kdebug("Unable to connect, the following error has occured:\n%s\n", strerror(errno));
	kadu->disconnectNetwork();
	if (kadu->autohammer)
		AutoConnectionTimer::on();
};

void EventManager::connectionBrokenSlot()
{
	kdebug("Connection broken unexpectedly!\nUnscheduled connection termination\n");
	kadu->disconnectNetwork();
//	kadu->setCurrentStatus(GG_STATUS_NOT_AVAIL);
	if (kadu->autohammer)
		AutoConnectionTimer::on();
};

void EventManager::disconnectedSlot()
{
	trayicon->showErrorHint(i18n("Disconnection has occured"));
	kdebug("Disconnection been occured\n");
	kadu->autohammer = false;
	kadu->disconnectNetwork();
	AutoConnectionTimer::off();
// Wykomentowa³em, bo to zawsze jest prawdziwe!
/*	if (e->type == GG_EVENT_DISCONNECT) */
};

void EventManager::messageReceivedSlot(int msgclass, UinsList senders,unsigned char* msg, time_t time,
	int formats_length, void *formats)
{

/* Moim zdaniem t± ca³a funkcje trzeba przepisaæ od nowa,
	a przynajmniej poprawiæ i rozbiæ na mniejsze.
*/
/*
	sprawdzamy czy user jest na naszej liscie, jezeli nie to .anonymous zwroci true
	i czy jest wlaczona opcja ignorowania nieznajomych
	jezeli warunek jest spelniony przerywamy dzialanie funkcji.
*/
	if (userlist.byUinValue(senders[0]).anonymous && config_file.readBoolEntry("Other","IgnoreAnonymousUsers")) {
		kdebug("EventManager::messageReceivedSlot(): Ignored anonymous. %d is ignored\n",senders[0]);
		return;
		}
/*
	w dalszej czêsci kodu wystêpuje ten warunek, po co ma sprawdzac po kilka razy, jak mozna raz,
	podejrzewam ze to juz nie jest potrzebne(ale na wszelki wypadek zostawie), gdyz kiedys to
	sluzylo co oznaczania wiadomosci systemowych,ktore obecnie sa ignorowane, w dalszej czesci kodu.
*/
	if (senders[0] == config_file.readNumEntry("Global","UIN"))
		return;

	// ignorujemy, jesli nick na liscie ignorowanych
	// PYTANIE CZY IGNORUJEMY CALA KONFERENCJE
	// JESLI PIERWSZY SENDER JEST IGNOROWANY????
	if (isIgnored(senders))
		return;

	// ignorujemy wiadomosci systemowe
	if (senders[0] == 0)
	{
		if (msgclass <= config_file.readNumEntry("Global","SystemMsgIndex"))
		{
			kdebug("Already had this message, ignoring\n");
			return;
		}
		config_file.writeEntry("Global","SystemMsgIndex",msgclass);
		kdebug("System message index %d\n", msgclass);
		return;
	}

	QString mesg = cp2unicode(msg);

	int i;

#ifdef HAVE_OPENSSL
	if (config_file.readBoolEntry("Other","Encryption")) {
		if (!strncmp((char *)msg, "-----BEGIN RSA PUBLIC KEY-----", 20)) {
			QFile keyfile;
			QString keyfile_path;
			QWidget *parent;

			i = 0;
			while (i < chats.count() && !chats[i].uins.equals(senders))
				i++;
			if (i == chats.count())
				parent = kadu;
			else
				parent = chats[i].ptr;

			SavePublicKey *spk = new SavePublicKey(senders[0], mesg, NULL);
			spk->show();
			return;
		}
	};

	if (msg != NULL)	{
		kdebug("Decrypting encrypted message...\n");
		char* decoded = sim_message_decrypt(msg, senders[0]);
		kdebug("Decrypted message is: %s\n",decoded);
		if (decoded != NULL)
			strcpy((char *)msg, decoded);
		mesg = cp2unicode(msg);
		}
#endif

	mesg = formatGGMessage(mesg, formats_length, formats);

	UserListElement ule = userlist.byUinValue(senders[0]);
/*
	Sprawdzamy czy uin znajduje sie na naszej userliscie, jezeli nie to w zalezno¶ci czy
	mamy w³±czon± ikonke w trayu to dodajemy tylko do userlisty(ale chyba nie zapisujemy)
	lub dodajemy automatycznie do naszej userlisty.
*/

 
 	//script.eventMsg(senders[0],msgclass,(char*)msg);
	
		if (config_file.readBoolEntry("Global","UseDocking"))
			userlist.addUser("", "", ule.altnick, ule.altnick, "", ule.altnick, GG_STATUS_NOT_AVAIL,
				0, false, false, true, "", "", "", true);
		else
			kadu->addUser("", "", ule.altnick, ule.altnick, "", ule.altnick, GG_STATUS_NOT_AVAIL,
				0, "", "", "", true);

	if (config_file.readBoolEntry("Global","Logging"))	
		history.appendMessage(senders, senders[0], mesg, FALSE, time);

	//script.eventMsg(senders[0],msgclass,(char*)msg);

	i = 0;
	while (i < chats.count() && !chats[i].uins.equals(senders))
		i++;

	if (((msgclass & GG_CLASS_CHAT) == GG_CLASS_CHAT || (msgclass & GG_CLASS_MSG) == GG_CLASS_MSG
		|| !msgclass) && i < chats.count()) {
		QString toadd;

		chats[i].ptr->checkPresence(senders, mesg, time, toadd);
		chats[i].ptr->alertNewMessage();
		if (!chats[i].ptr->isActiveWindow() && config_file.readBoolEntry("Other","HintAlert"))
			trayicon->showHint(i18n("New message from: "), ule.altnick,0);
		return;
		}

	playSound(parse(config_file.readEntry("Global","Message_sound"),ule));

	pending.addMsg(senders, mesg, msgclass, time);
	
	kdebug("eventRecvMsg(): Message allocated to slot %d\n", i);
	kdebug("eventRecvMsg(): Got message from %d (%s) saying \"%s\"\n",
			senders[0], (const char *)ule.altnick.local8Bit(), (const char *)mesg.local8Bit());

	UserBox::all_refresh();
	trayicon->changeIcon();

	if (config_file.readBoolEntry("Global","AutoRaise")) {
		kadu->showNormal();
		kadu->setFocus();
		}

	if (msgclass == GG_CLASS_CHAT)
		trayicon->showHint(i18n("Chat with: "), ule.altnick,0);
	if (msgclass == GG_CLASS_MSG)
		trayicon->showHint(i18n("Message from: "), ule.altnick,0);

	emit chatReceived(senders,mesg,time);

/*	PendingMsgs::Element elem;

	if (senders[0] == config.uin) {
		rMessage *rmsg;
		elem = pending[i];
		rmsg = new rMessage("System", elem.msgclass, elem.uins, elem.msg);
		rmsg->init();
		rmsg->show();
		}*/
}

void EventManager::chatReceivedSlot(UinsList senders,const QString& msg,time_t time)
{
	if(config_file.readBoolEntry("Other","OpenChatOnMessage"))
		pending.openMessages();
};

void ifNotify(uin_t uin, unsigned int status, unsigned int oldstatus)
{
	if (userlist.containsUin(uin)) {
		UserListElement ule = userlist.byUin(uin);
		if (!ule.notify && !config_file.readBoolEntry("Notify","NotifyAboutAll"))
			return;
		}
	else
		if (!config_file.readBoolEntry("Notify","NotifyAboutAll"))
			return;

	if (config_file.readBoolEntry("Notify","NotifyStatusChange") && (status == GG_STATUS_AVAIL ||
		status == GG_STATUS_AVAIL_DESCR || status == GG_STATUS_BUSY || status == GG_STATUS_BUSY_DESCR
		|| status == GG_STATUS_BLOCKED) &&
		(oldstatus == GG_STATUS_NOT_AVAIL || oldstatus == GG_STATUS_NOT_AVAIL_DESCR || oldstatus == GG_STATUS_INVISIBLE ||
		oldstatus == GG_STATUS_INVISIBLE_DESCR || oldstatus == GG_STATUS_INVISIBLE2)) {
		kdebug("Notify about user\n");

		if (config_file.readBoolEntry("Notify","NotifyWithDialogBox")) {		
			// FIXME convert into a regular QMessageBox
			QString msg;
			msg = i18n("User %1 is available").arg(userlist.byUin(uin).altnick);
			QMessageBox *msgbox;
			msgbox = new QMessageBox(i18n("User notify"), msg, QMessageBox::NoIcon,
				QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton,
				0, 0, FALSE, Qt::WStyle_DialogBorder || Qt::WDestructiveClose);
			msgbox->show();
			}



		if (config_file.readBoolEntry("Notify","NotifyWithSound")) {
			if (lastsoundtime.elapsed() >= 500)
				playSound(parse(config_file.readEntry("Notify","NotifySound"),userlist.byUin(uin),false));
			lastsoundtime.restart();
			}
		if (config_file.readBoolEntry("Notify","NotifyWithHint"))		
			trayicon->showHint(i18n(" is available"),userlist.byUin(uin).altnick,1);
		}
}

void EventManager::userlistReceivedSlot(struct gg_event *e) {
	unsigned int oldstatus;
	int i, nr = 0;

	while (e->event.notify60[nr].uin) {
		UserListElement &user = userlist.byUin(e->event.notify60[nr].uin);

		if (!userlist.containsUin(e->event.notify60[nr].uin)) {
			kdebug("eventGotUserlist(): buddy %d not in list. Damned server!\n",
				e->event.notify60[nr].uin);
			gg_remove_notify(sess, e->event.notify60[nr].uin);
			nr++;
			continue;
			}

		user.ip.setAddress(ntohl(e->event.notify60[nr].remote_ip));
		userlist.addDnsLookup(user.uin, user.ip);
		user.port = e->event.notify60[nr].remote_port;
		user.version = e->event.notify60[nr].version;
		user.image_size = e->event.notify60[nr].image_size;

		oldstatus = user.status;

		if (user.description)
			user.description.truncate(0);

		if (e->event.notify60[nr].descr)
			user.description.append(cp2unicode((unsigned char *)e->event.notify60[nr].descr));

		switch (e->event.notify60[nr].status) {
			case GG_STATUS_AVAIL:
				kdebug("eventGotUserlist(): User %d went online\n",
					e->event.notify60[nr].uin);
				break;
			case GG_STATUS_BUSY:
				kdebug("eventGotUserlist(): User %d went busy\n",
					e->event.notify60[nr].uin);
				break;
			case GG_STATUS_NOT_AVAIL:
				kdebug("eventGotUserlist(): User %d went offline\n",
					e->event.notify60[nr].uin);
				break;
			case GG_STATUS_BLOCKED:
				kdebug("eventGotUserlist(): User %d has blocked us\n",
					e->event.notify60[nr].uin);
				break;
			case GG_STATUS_BUSY_DESCR:
				kdebug("eventGotUserlist(): User %d went busy with descr.\n",
					e->event.notify60[nr].uin);
				break;
			case GG_STATUS_NOT_AVAIL_DESCR:
				kdebug("eventGotUserlist(): User %d went offline with descr.\n",
					e->event.notify60[nr].uin);
				break;
			case GG_STATUS_AVAIL_DESCR:
				kdebug("eventGotUserlist(): User %d went online with descr.\n",
					e->event.notify60[nr].uin);
				break;
			case GG_STATUS_INVISIBLE_DESCR:
				kdebug("eventGotUserlist(): User %d went invisible with descr.\n",
					e->event.notify60[nr].uin);
				break;
			default:
				kdebug("eventGotUserlist(): Unknown status for user %d: %d\n",
					e->event.notify60[nr].uin, e->event.notify60[nr].status);
				break;
			}
		userlist.changeUserStatus(e->event.notify60[nr].uin, e->event.notify60[nr].status);

		history.appendStatus(user.uin, user.status, user.description.length() ? user.description : QString::null);

		for (i = 0; i < chats.count(); i++)
			if (chats[i].uins.contains(e->event.notify60[nr].uin))
				chats[i].ptr->setTitle();

		ifNotify(e->event.notify60[nr].uin, e->event.notify60[nr].status, oldstatus);

		nr++;		
		}
	UserBox::all_refresh();
}

void EventManager::userStatusChangedSlot(struct gg_event * e) {
	unsigned int oldstatus, status;
	int i;
	uint32_t uin;
	char *descr;
	uint32_t remote_ip;
	uint16_t remote_port;
	uint8_t version;
	uint8_t image_size;
	
	if (e->type == GG_EVENT_STATUS60) {
		uin = e->event.status60.uin;
		status = e->event.status60.status;
		descr = e->event.status60.descr;
		remote_ip = e->event.status60.remote_ip;
		remote_port = e->event.status60.remote_port;
		version = e->event.status60.version;
		image_size = e->event.status60.image_size;
		}
	else {
		uin = e->event.status.uin;
		status = e->event.status.status;
		descr = e->event.status.descr;
		remote_ip = 0;
		remote_port = 0;
		version = 0;
		image_size = 0;
		}

	kdebug("eventStatusChange(): User %d went %d\n", uin,  status);
	UserListElement &user = userlist.byUin(uin);

	if (!userlist.containsUin(uin)) {
		// ignore!
		kdebug("eventStatusChange(): buddy %d not in list. Damned server!\n", uin);
		gg_remove_notify(sess, uin);
		return;
		}

	oldstatus = user.status;

	if (user.description)
		user.description.truncate(0);
//	if (ifStatusWithDescription(e->event.status.status)) {
	if (descr)
		user.description.append(cp2unicode((unsigned char *)descr));
	userlist.changeUserStatus(uin, status);
	
	if (user.status == GG_STATUS_NOT_AVAIL || user.status == GG_STATUS_NOT_AVAIL_DESCR) {
		user.ip.setAddress((unsigned int)0);
		userlist.addDnsLookup(user.uin, user.ip);
		user.port = 0;
		user.version = 0;
		user.image_size = 0;
		}
	else {
		user.ip.setAddress(ntohl(remote_ip));
		userlist.addDnsLookup(user.uin, user.ip);
		user.port = remote_port;
		user.version = version;
		user.image_size = image_size;
		}

	history.appendStatus(user.uin, user.status, user.description.length() ? user.description : QString::null);

	for (i = 0; i < chats.count(); i++)
		if (chats[i].uins.contains(uin))
			chats[i].ptr->setTitle();
			
	ifNotify(uin, status, oldstatus);
	UserBox::all_refresh();
};

void EventManager::ackReceivedSlot(int seq)
{
	kdebug("EventManager::ackReceivedSlot(): got msg ack.\n");
};

void EventManager::dccConnectionReceivedSlot(const UserListElement& sender)
{
	struct gg_dcc *dcc_new;
	dccSocketClass *dcc;
	if (dccSocketClass::count < 8)
	{
		dcc_new = gg_dcc_get_file(htonl(sender.ip.ip4Addr()), sender.port, config_file.readNumEntry("Global","UIN"), sender.uin);
		if (dcc_new)
		{
			dcc = new dccSocketClass(dcc_new);
			connect(dcc, SIGNAL(dccFinished(dccSocketClass *)), this, SLOT(dccFinished(dccSocketClass *)));
			dcc->initializeNotifiers();
		}
	}
};

void EventManager::pubdirReplyReceivedSlot(gg_pubdir50_t res)
{
	kdebug("EventManager::pubdirReplyReceivedSlot(): got pubdir reply.\n");
};

void EventManager::userlistReplyReceivedSlot(char type, char *reply)
{
	kdebug("EventManager::userlistReplyReceivedSlot(): got userlist reply.\n");
}

void EventManager::eventHandler(gg_session* sess)
{
	static int calls = 0;

	kdebug("EventManager::eventHandler()\n");
	calls++;
	if (calls > 1)
		kdebug("************* EventManager::eventHandler(): Recursive eventHandler calls detected!\n");

	gg_event* e;
	if (!(e = gg_watch_fd(sess))) {
		emit connectionBroken();
		gg_free_event(e);
		calls--;
		return;
		}

	if (sess->state == GG_STATE_CONNECTING_HUB || sess->state == GG_STATE_CONNECTING_GG)
	{
		kdebug("EventManager::eventHandler(): changing QSocketNotifiers.\n");

		kadusnw->setEnabled(false);
		delete kadusnw;

		kadusnr->setEnabled(false);
		delete kadusnr;

		kadusnw = new QSocketNotifier(sess->fd, QSocketNotifier::Write, this); 
		QObject::connect(kadusnw, SIGNAL(activated(int)), kadu, SLOT(dataSent()));

		kadusnr = new QSocketNotifier(sess->fd, QSocketNotifier::Read, this); 
		QObject::connect(kadusnr, SIGNAL(activated(int)), kadu, SLOT(dataReceived()));    
	};

	switch (sess->state)
	{
		case GG_STATE_RESOLVING:
			kdebug("EventManager::eventHandler(): Resolving address\n");
			break;
		case GG_STATE_CONNECTING_HUB:
			kdebug("EventManager::eventHandler(): Connecting to hub\n");
			break;
		case GG_STATE_READING_DATA:
			kdebug("EventManager::eventHandler(): Fetching data from hub\n");
			break;
		case GG_STATE_CONNECTING_GG:
			kdebug("EventManager::eventHandler(): Connecting to server\n");
			break;
		case GG_STATE_READING_KEY:
			kdebug("EventManager::eventHandler(): Waiting for hash key\n");
			break;
		case GG_STATE_READING_REPLY:
			kdebug("EventManager::eventHandler(): Sending key\n");
			break;
		case GG_STATE_CONNECTED:
			break;
		default:
			break;
	}

	if (sess->check == GG_CHECK_READ) {
		timeout_connected = true;
		last_read_event = time(NULL);
		}

	if (e->type == GG_EVENT_MSG) {
		if (e->event.msg.msgclass == GG_CLASS_CTCP)
			emit dccConnectionReceived(userlist.byUin(e->event.msg.sender));
		else {
			UinsList uins;
			kdebug("eventHandler(): %d\n", e->event.msg.recipients_count);
			if ((e->event.msg.msgclass & GG_CLASS_CHAT) == GG_CLASS_CHAT) {
				uins.append(e->event.msg.sender);	
				for (int i = 0; i < e->event.msg.recipients_count; i++)
					uins.append(e->event.msg.recipients[i]);
				}
			else
				uins.append(e->event.msg.sender);				
			emit event_manager.messageReceived(e->event.msg.msgclass, uins, e->event.msg.message,
				e->event.msg.time, e->event.msg.formats_length, e->event.msg.formats);
			}
		}

	if (e->type == GG_EVENT_STATUS60 || e->type == GG_EVENT_STATUS)
		emit event_manager.userStatusChanged(e);

	if (e->type == GG_EVENT_ACK) {
		kdebug("EventManager::eventHandler(): message reached %d (seq %d)\n",
			e->event.ack.recipient, e->event.ack.seq);
		emit ackReceived(e->event.ack.seq);
		}

	if (e->type == GG_EVENT_NOTIFY60)
		emit event_manager.userlistReceived(e);
	
	if (e->type == GG_EVENT_PUBDIR50_SEARCH_REPLY
		|| e->type == GG_EVENT_PUBDIR50_READ || e->type == GG_EVENT_PUBDIR50_WRITE)
		emit pubdirReplyReceived(e->event.pubdir50);
	
	if (e->type == GG_EVENT_USERLIST)
		emit event_manager.userlistReplyReceived(e->event.userlist.type,
			e->event.userlist.reply);

	if (e->type == GG_EVENT_CONN_SUCCESS)
		emit connected();

	if (e->type == GG_EVENT_CONN_FAILED || e->type == GG_EVENT_DISCONNECT)
		emit connectionFailed();

	if (e->type == GG_EVENT_DISCONNECT)
		emit disconnected();

	if (socket_active) {
		if (sess->state == GG_STATE_IDLE && userlist_sent) {
			socket_active = false;
			UserBox::all_changeAllToInactive();
			emit connectionBroken();
			}
		else
			if (sess->check & GG_CHECK_WRITE)
				kadusnw->setEnabled(true);
		}

	gg_free_event(e);
	calls--;
};

EventManager event_manager;
