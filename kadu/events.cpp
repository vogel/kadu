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
#include <qfiledialog.h>
#include <qlayout.h>
#include <qmessagebox.h>
#include <qregexp.h>

#include <netinet/in.h>
#include <errno.h>
#include <time.h>

#include "kadu.h"
#include "message_box.h"
#include "ignore.h"
#include "events.h"
#include "chat.h"
#include "history.h"
#include "pending_msgs.h"
#include "hints.h"
#include "dock_widget.h"
#include "debug.h"
#include "sound.h"
#include "dcc.h"
#include "config_dialog.h"
#include "config_file.h"
#include "../config.h"
#ifdef HAVE_OPENSSL
extern "C"
{
#include "simlite.h"
};
#endif

AutoConnectionTimer *AutoConnectionTimer::autoconnection_object = NULL;
ConnectionTimeoutTimer *ConnectionTimeoutTimer::connectiontimeout_object = NULL;

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

ConnectionTimeoutTimer::ConnectionTimeoutTimer(QObject *parent) : QTimer(parent, "ConnectionTimeoutTimer") {
	start(3000, TRUE);
}

bool ConnectionTimeoutTimer::connectTimeoutRoutine(const QObject *receiver, const char *member) {
	return connect(connectiontimeout_object, SIGNAL(timeout()), receiver, member);
}

void ConnectionTimeoutTimer::on() {
	if (!connectiontimeout_object)
		connectiontimeout_object = new ConnectionTimeoutTimer();
}

void ConnectionTimeoutTimer::off() {
	if (connectiontimeout_object) {
		delete connectiontimeout_object;
		connectiontimeout_object = NULL;
		}
}

SavePublicKey::SavePublicKey(uin_t uin, QString keyData, QWidget *parent, const char *name) :
	QDialog(parent, name, Qt::WDestructiveClose), uin(uin), keyData(keyData) {
	
	kdebug("SavePublicKey::SavePublicKey()\n");

	setCaption(tr("Save public key"));
	resize(200, 80);
	
	QLabel *l_info = new QLabel(
		tr("User %1 is sending you his public key. Do you want to save it?").arg(userlist.byUin(uin).altnick),
		this);

	QPushButton *yesbtn = new QPushButton(tr("Yes"), this);
	QPushButton *nobtn = new QPushButton(tr("No"), this);

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
		QMessageBox::critical(this, tr("Error"), tr("Error writting the key"), tr("OK"), QString::null, 0);
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
	connect(this,SIGNAL(connectionFailed(int)),this,SLOT(connectionFailedSlot(int)));
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
	if (config_file.readBoolEntry("General","AutoAway"))
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

void EventManager::connectionFailedSlot(int failure)
{
	QString msg = QString::null;

	kadu->disconnectNetwork(); /* FIXME 1/2 */
	switch (failure) {
		case GG_FAILURE_RESOLVING:
			msg = QString(tr("Unable to connect, server has not been found"));
			break;
		case GG_FAILURE_CONNECTING:
			msg = QString(tr("Unable to connect"));
			break;
		case GG_FAILURE_NEED_EMAIL:
			msg = QString(tr("Please change your email in \"Change password/email\" window. "
				"Leave new password field blank."));
			MessageBox::msg(msg);
			break;
		case GG_FAILURE_INVALID:
			msg = QString(tr("Unable to connect, server has returned unknown data"));
			break;
		case GG_FAILURE_READING:
			msg = QString(tr("Unable to connect, connection break during reading"));
			break;
		case GG_FAILURE_WRITING:
			msg = QString(tr("Unable to connect, connection break during writing"));
			break;
		case GG_FAILURE_PASSWORD:
			msg = QString(tr("Unable to connect, incorrect password"));
			kadu->autohammer = false; /* FIXME 2/2*/
			AutoConnectionTimer::off();
			hintmanager->addHintError(msg);
			QMessageBox::critical(0, tr("Incorrect password"), tr("Connection will be stoped\nYour password is incorrect !!!"), QMessageBox::Ok, 0);
			return;
		case GG_FAILURE_TLS:
			msg = QString(tr("Unable to connect, error of negotiation TLS"));
			break;
		}
	if (msg != QString::null) {
		kdebug("%s\n", unicode2latin(msg).data());
		hintmanager->addHintError(msg);
		}
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
	if (hintmanager != NULL)
		hintmanager->addHintError(tr("Disconnection has occured"));
	kdebug("Disconnection has occured\n");
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
	if (userlist.byUinValue(senders[0]).anonymous && config_file.readBoolEntry("Chat","IgnoreAnonymousUsers")) {
		kdebug("EventManager::messageReceivedSlot(): Ignored anonymous. %d is ignored\n",senders[0]);
		return;
		}
/*
	w dalszej czêsci kodu wystêpuje ten warunek, po co ma sprawdzac po kilka razy, jak mozna raz,
	podejrzewam ze to juz nie jest potrzebne(ale na wszelki wypadek zostawie), gdyz kiedys to
	sluzylo co oznaczania wiadomosci systemowych,ktore obecnie sa ignorowane, w dalszej czesci kodu.
*/
	if (senders[0] == config_file.readNumEntry("General","UIN"))
		return;

	// ignorujemy, jesli nick na liscie ignorowanych
	// PYTANIE CZY IGNORUJEMY CALA KONFERENCJE
	// JESLI PIERWSZY SENDER JEST IGNOROWANY????
	if (isIgnored(senders))
		return;

	// ignorujemy wiadomosci systemowe
	if (senders[0] == 0)
	{
		if (msgclass <= config_file.readNumEntry("General","SystemMsgIndex",0))
		{
			kdebug("Already had this message, ignoring\n");
			return;
		}
		config_file.writeEntry("General","SystemMsgIndex",msgclass);
		kdebug("System message index %d\n", msgclass);
		return;
	}

	QString mesg = cp2unicode(msg);

	int i;

#ifdef HAVE_OPENSSL
	if (config_file.readBoolEntry("Chat","Encryption")) {
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
 
	UserListElement e;
	bool ok;

	if(!userlist.containsUin(senders[0])) {
		e.first_name = "";
		e.last_name = "";
		e.nickname = ule.altnick;
		e.altnick = ule.altnick;
		e.uin = ule.altnick.toUInt(&ok);
		if (!ok)
			e.uin = 0;
		e.mobile = "";
		e.setGroup("");
		e.description = "";
		e.email = "";
		e.anonymous = true;
		if (config_file.readBoolEntry("General", "UseDocking"))
			userlist.addUser(e);
		else
			kadu->addUser(e);
		}

	if (config_file.readBoolEntry("General","Logging"))	
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
		if (!chats[i].ptr->isActiveWindow() && config_file.readBoolEntry("Hints","NotifyNewMessage"))
			hintmanager->addHintNewMsg(ule.altnick, mesg);
		return;
		}

	playSound(parse(config_file.readEntry("Sounds","Message_sound"),ule));

	pending.addMsg(senders, mesg, msgclass, time);
	
	kdebug("eventRecvMsg(): Message allocated to slot %d\n", i);
	kdebug("eventRecvMsg(): Got message from %d (%s) saying \"%s\"\n",
			senders[0], (const char *)ule.altnick.local8Bit(), (const char *)mesg.local8Bit());

	UserBox::all_refresh();
	trayicon->changeIcon();

	if (config_file.readBoolEntry("General","AutoRaise")) {
		kadu->showNormal();
		kadu->setFocus();
		}

	if ((msgclass == GG_CLASS_CHAT || msgclass == GG_CLASS_MSG))
		hintmanager->addHintNewChat(ule.altnick, mesg);

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
	if(config_file.readBoolEntry("Chat","OpenChatOnMessage"))
		pending.openMessages();
};

void ifNotify(uin_t uin, unsigned int status, unsigned int oldstatus)
{
	if (!config_file.readBoolEntry("Notify","NotifyStatusChange"))
			return;

	if (userlist.containsUin(uin)) {
		UserListElement ule = userlist.byUin(uin);
		if (!ule.notify && !config_file.readBoolEntry("Notify","NotifyAboutAll"))
			return;
		}
	else
		if (!config_file.readBoolEntry("Notify","NotifyAboutAll"))
			return;

	if (config_file.readBoolEntry("Hints","NotifyHint"))
		hintmanager->addHintStatus(userlist.byUinValue(uin), status, oldstatus);

	if (config_file.readBoolEntry("Notify","NotifyStatusChange") && (status == GG_STATUS_AVAIL ||
		status == GG_STATUS_AVAIL_DESCR || status == GG_STATUS_BUSY || status == GG_STATUS_BUSY_DESCR
		|| status == GG_STATUS_BLOCKED) &&
		(oldstatus == GG_STATUS_NOT_AVAIL || oldstatus == GG_STATUS_NOT_AVAIL_DESCR || oldstatus == GG_STATUS_INVISIBLE ||
		oldstatus == GG_STATUS_INVISIBLE_DESCR || oldstatus == GG_STATUS_INVISIBLE2)) {
		kdebug("Notify about user\n");

		if (config_file.readBoolEntry("Notify","NotifyWithDialogBox")) {
			// FIXME convert into a regular QMessageBox
			QString msg;
			msg = QT_TR_NOOP(QString("User %1 is available")).arg(userlist.byUin(uin).altnick);
			QMessageBox *msgbox;
			msgbox = new QMessageBox(QT_TR_NOOP("User notify"), msg, QMessageBox::NoIcon,
				QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton,
				0, 0, FALSE, Qt::WStyle_DialogBorder || Qt::WDestructiveClose);
			msgbox->show();
			}

		if (config_file.readBoolEntry("Notify","NotifyWithSound") && config_file.readBoolEntry("Sounds", "PlaySound")) {
			if (lastsoundtime.elapsed() >= 500)
				playSound(parse(config_file.readEntry("Notify","NotifySound"),userlist.byUin(uin),false));
			lastsoundtime.restart();
			}
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
		dcc_new = gg_dcc_get_file(htonl(sender.ip.ip4Addr()), sender.port, config_file.readNumEntry("General","UIN"), sender.uin);
		if (dcc_new)
		{
			dcc = new dccSocketClass(dcc_new);
			connect(dcc, SIGNAL(dccFinished(dccSocketClass *)), kadu, SLOT(dccFinished(dccSocketClass *)));
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

void EventManager::connectionTimeoutSlot() {
	ConnectionTimeoutTimer::off();
	if (sess->state != GG_STATE_CONNECTING_GG || sess->port == GG_HTTPS_PORT)
		return;
	gg_event* e;
	sess->timeout = 0;
	if (!(e = gg_watch_fd(sess))) {
		emit connectionBroken();
		gg_free_event(e);
		return;
		}
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
			ConnectionTimeoutTimer::on();
			ConnectionTimeoutTimer::connectTimeoutRoutine(this,
				SLOT(connectionTimeoutSlot()));
			break;
		case GG_STATE_READING_KEY:
			kdebug("EventManager::eventHandler(): Waiting for hash key\n");
			ConnectionTimeoutTimer::off();
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

	if (e->type == GG_EVENT_CONN_FAILED)
		emit connectionFailed(e->event.failure);

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

void EventConfigSlots::initModule()
{
    	kdebug("EventConfigSlots::initModule()\n");
	
	EventConfigSlots *eventconfigslots = new EventConfigSlots();
	


	QT_TRANSLATE_NOOP("@default", "Notify");
	QT_TRANSLATE_NOOP("@default", "Notify when users become available");
	QT_TRANSLATE_NOOP("@default", "Notify about all users");
	QT_TRANSLATE_NOOP("@default", "Available");
	QT_TRANSLATE_NOOP("@default", "Tracked");
	QT_TRANSLATE_NOOP("@default", "Notify options");
	QT_TRANSLATE_NOOP("@default", "Notify by sound");
	QT_TRANSLATE_NOOP("@default", "Path:");
	QT_TRANSLATE_NOOP("@default", "Test");
	QT_TRANSLATE_NOOP("@default", "Notify by dialog box");
	QT_TRANSLATE_NOOP("@default", "Notify sound");

// zakladka "powiadom"
	ConfigDialog::addTab("Notify");
	ConfigDialog::addCheckBox("Notify", "Notify", "Notify when users become available", "NotifyStatusChange", false);
	ConfigDialog::addCheckBox("Notify", "Notify", "Notify about all users", "NotifyAboutAll", false);
	ConfigDialog::addGrid("Notify", "Notify" ,"listboxy",3);
	
	ConfigDialog::addGrid("Notify", "listboxy", "listbox1", 1);
	ConfigDialog::addLabel("Notify", "listbox1", "Available");
	ConfigDialog::addListBox("Notify", "listbox1","available");
	
	ConfigDialog::addGrid("Notify", "listboxy", "listbox2", 1);
	ConfigDialog::addPushButton("Notify", "listbox2", "", "forward.png","","forward");
	ConfigDialog::addPushButton("Notify", "listbox2", "", "back.png","","back");
	
	ConfigDialog::addGrid("Notify", "listboxy", "listbox3", 1);
	ConfigDialog::addLabel("Notify", "listbox3", "Tracked");
	ConfigDialog::addListBox("Notify", "listbox3", "track");
	
	ConfigDialog::addVGroupBox("Notify", "Notify", "Notify options");
	ConfigDialog::addCheckBox("Notify", "Notify options", "Notify by sound", "NotifyWithSound", false);
	
	ConfigDialog::addHGroupBox("Notify", "Notify options","Notify sound");
	ConfigDialog::addLineEdit("Notify", "Notify sound", "Path:", "NotifySound");
	ConfigDialog::addPushButton("Notify", "Notify sound","","fileopen.png","","notifysoundfile");
	ConfigDialog::addPushButton("Notify", "Notify sound", "Test");
	ConfigDialog::addCheckBox("Notify", "Notify options", "Notify by dialog box", "NotifyWithDialogBox", false);
	

//zakladka "siec"
	//potrzebne do translacji
	QT_TRANSLATE_NOOP("@default", "Network");
	QT_TRANSLATE_NOOP("@default",  "DCC enabled");
	QT_TRANSLATE_NOOP("@default", "DCC IP autodetection");
	QT_TRANSLATE_NOOP("@default", "DCC IP");
	QT_TRANSLATE_NOOP("@default", "IP address:");
	QT_TRANSLATE_NOOP("@default", "DCC forwarding enabled");
	QT_TRANSLATE_NOOP("@default", "DCC forwarding properties");
	QT_TRANSLATE_NOOP("@default", "External IP address:");
	QT_TRANSLATE_NOOP("@default", "External TCP port:");
	QT_TRANSLATE_NOOP("@default", "Servers properties");
	QT_TRANSLATE_NOOP("@default", "Use default servers");
	QT_TRANSLATE_NOOP("@default", "Use TLSv1");
	QT_TRANSLATE_NOOP("@default", "Default port to connect to servers");
	QT_TRANSLATE_NOOP("@default", "Use proxy server");
	QT_TRANSLATE_NOOP("@default", "Proxy server");
	QT_TRANSLATE_NOOP("@default", "Port:");
	QT_TRANSLATE_NOOP("@default", "IP addresses:");
	QT_TRANSLATE_NOOP("@default", "Username:");
	QT_TRANSLATE_NOOP("@default", "Password:");


	ConfigDialog::addTab("Network");
	ConfigDialog::addCheckBox("Network", "Network", "DCC enabled", "AllowDCC", false);
	ConfigDialog::addCheckBox("Network", "Network", "DCC IP autodetection", "DccIpDetect", false);
	
	ConfigDialog::addVGroupBox("Network", "Network", "DCC IP");
	ConfigDialog::addLineEdit("Network", "DCC IP", "IP address:","DccIP");
	ConfigDialog::addCheckBox("Network", "Network", "DCC forwarding enabled", "DccForwarding", false);
	
	ConfigDialog::addVGroupBox("Network", "Network", "DCC forwarding properties");
	ConfigDialog::addLineEdit("Network", "DCC forwarding properties", "External IP address:", "ExternalIP");
	ConfigDialog::addLineEdit("Network", "DCC forwarding properties", "External TCP port:", "ExternalPort", "0");

	ConfigDialog::addVGroupBox("Network", "Network", "Servers properties");
	ConfigDialog::addGrid("Network", "Servers properties", "servergrid", 2);
	ConfigDialog::addCheckBox("Network", "servergrid", "Use default servers", "isDefServers", true);
#ifdef HAVE_OPENSSL
	ConfigDialog::addCheckBox("Network", "servergrid", "Use TLSv1", "UseTLS", false);
#endif
	ConfigDialog::addLineEdit("Network", "Servers properties", "IP addresses:", "Server","","","server");
#ifdef HAVE_OPENSSL
	ConfigDialog::addComboBox("Network", "Servers properties", "Default port to connect to servers");
#endif
	ConfigDialog::addCheckBox("Network", "Network", "Use proxy server", "UseProxy", false);

	ConfigDialog::addVGroupBox("Network", "Network", "Proxy server");
	ConfigDialog::addGrid("Network", "Proxy server", "proxygrid", 2);
	ConfigDialog::addLineEdit("Network", "proxygrid", "IP address:", "ProxyHost", "","","proxyhost");
	ConfigDialog::addLineEdit("Network", "proxygrid", "Port:", "ProxyPort", "0");
	ConfigDialog::addLineEdit("Network", "proxygrid", "Username:", "ProxyUser");
	ConfigDialog::addLineEdit("Network", "proxygrid", "Password:", "ProxyPassword");
	
	ConfigDialog::registerSlotOnCreate(eventconfigslots, SLOT(onCreateConfigDialog()));
	ConfigDialog::registerSlotOnDestroy(eventconfigslots, SLOT(onDestroyConfigDialog()));
	
	ConfigDialog::connectSlot("Network", "DCC enabled", SIGNAL(toggled(bool)), eventconfigslots, SLOT(ifDccEnabled(bool)));
	ConfigDialog::connectSlot("Network", "DCC IP autodetection", SIGNAL(toggled(bool)), eventconfigslots, SLOT(ifDccIpEnabled(bool)));
	ConfigDialog::connectSlot("Network", "Use default servers", SIGNAL(toggled(bool)), eventconfigslots, SLOT(ifDefServerEnabled(bool)));
#ifdef HAVE_OPENSSL
	ConfigDialog::connectSlot("Network", "Use TLSv1", SIGNAL(toggled(bool)), eventconfigslots, SLOT(useTlsEnabled(bool)));
#endif	

	    defaultdescriptions = QStringList::split(QRegExp("<-->"), config_file.readEntry("General","DefaultDescription", tr("I am busy.")), true);
		if (!config_file.readBoolEntry("Network","DccIpDetect"))
		if (!config_dccip.setAddress(config_file.readEntry("Network","DccIP", "")))
			config_dccip.setAddress((unsigned int)0);

	        if (!config_extip.setAddress(config_file.readEntry("Network","ExternalIP", "")))
			config_extip.setAddress((unsigned int)0);


	    QStringList servers;
	    QHostAddress ip2;
	    servers = QStringList::split(";", config_file.readEntry("Network","Server", ""));
	    config_servers.clear();
	        for (int i = 0; i < servers.count(); i++)
		    {
		        if (ip2.setAddress(servers[i]))
  			       config_servers.append(ip2);
		    }
		server_nr = 0;


	ConfigDialog::connectSlot("Notify", "", SIGNAL(clicked()), eventconfigslots, SLOT(_Right()), "forward");
	ConfigDialog::connectSlot("Notify", "", SIGNAL(clicked()), eventconfigslots, SLOT(_Left()), "back");
	ConfigDialog::connectSlot("Notify", "available", SIGNAL(doubleClicked(QListBoxItem *)), eventconfigslots, SLOT(_Right2(QListBoxItem *)));
	ConfigDialog::connectSlot("Notify", "track", SIGNAL(doubleClicked(QListBoxItem *)), eventconfigslots, SLOT(_Left2(QListBoxItem *)));
	ConfigDialog::connectSlot("Notify", "", SIGNAL(clicked()), eventconfigslots, SLOT(chooseNotifyFile()), "notifysoundfile");
	ConfigDialog::connectSlot("Notify", "Test", SIGNAL(clicked()), eventconfigslots, SLOT(chooseNotifyTest()));
}

void EventConfigSlots::onCreateConfigDialog()
{
	kdebug("EventConfigSlots::onCreateConfigDialog() \n");
	
	QCheckBox *b_dccenabled = ConfigDialog::getCheckBox("Network", "DCC enabled");
	QCheckBox *b_dccip= ConfigDialog::getCheckBox("Network", "DCC IP autodetection");
	QVGroupBox *g_dccip = ConfigDialog::getVGroupBox("Network", "DCC IP");
	QVGroupBox *g_proxy = ConfigDialog::getVGroupBox("Network", "Proxy server");
	QVGroupBox *g_fwdprop = ConfigDialog::getVGroupBox("Network", "DCC forwarding properties");
	QCheckBox *b_dccfwd = ConfigDialog::getCheckBox("Network", "DCC forwarding enabled");
	QCheckBox *b_useproxy= ConfigDialog::getCheckBox("Network", "Use proxy server");

#ifdef HAVE_OPENSSL
	QCheckBox *b_tls= ConfigDialog::getCheckBox("Network", "Use TLSv1");
	QComboBox *cb_portselect= ConfigDialog::getComboBox("Network", "Default port to connect to servers");
#endif

	QHBox *serverbox=(QHBox*)(ConfigDialog::getLineEdit("Network", "IP addresses:","server")->parent());
	QCheckBox* b_defaultserver= ConfigDialog::getCheckBox("Network", "Use default servers");
	
	b_dccip->setEnabled(b_dccenabled->isChecked());
	g_dccip->setEnabled(!b_dccip->isChecked()&& b_dccenabled->isChecked());
	b_dccfwd->setEnabled(b_dccenabled->isChecked());
	g_fwdprop->setEnabled(b_dccenabled->isChecked() && b_dccfwd->isChecked());
	g_proxy->setEnabled(b_useproxy->isChecked());

#ifdef HAVE_OPENSSL
	((QHBox*)cb_portselect->parent())->setEnabled(!b_tls->isChecked());
	cb_portselect->insertItem("8074");
	cb_portselect->insertItem("443");
	cb_portselect->setCurrentText(config_file.readEntry("Network", "DefaultPort", "8074"));	
#endif
	serverbox->setEnabled(!b_defaultserver->isChecked());
	
	connect(b_dccfwd, SIGNAL(toggled(bool)), g_fwdprop, SLOT(setEnabled(bool)));
        connect(b_useproxy, SIGNAL(toggled(bool)), g_proxy, SLOT(setEnabled(bool)));

// notify

	QListBox *e_availusers= ConfigDialog::getListBox("Notify", "available");
	QListBox *e_notifies= ConfigDialog::getListBox("Notify", "track");
	int i;
	i = 0;
	while (i < userlist.count()) {
		if (userlist[i].uin)
			if (!userlist[i].notify)
				e_availusers->insertItem(userlist[i].altnick);
			else
				e_notifies->insertItem(userlist[i].altnick);
		i++;
		}

	e_availusers->sort();
	e_notifies->sort();

	QCheckBox *b_notifyglobal= ConfigDialog::getCheckBox("Notify", "Notify when users become available");
	QCheckBox *b_notifyall= ConfigDialog::getCheckBox("Notify", "Notify about all users");
	QVGroupBox *notifybox= ConfigDialog::getVGroupBox("Notify", "Notify options");
	QGrid *panebox = ConfigDialog::getGrid("Notify","listboxy");	
	QHGroupBox *soundbox= ConfigDialog::getHGroupBox("Notify", "Notify sound");
	QCheckBox *b_notifysound= ConfigDialog::getCheckBox("Notify", "Notify by sound");
	
	if (config_file.readBoolEntry("Notify", "NotifyAboutAll")) 
		panebox->setEnabled(false);

	if (!config_file.readBoolEntry("Sounds", "PlaySound"))
		{
		soundbox->setEnabled(false);
		b_notifysound->setEnabled(false);
		}

	if (!config_file.readBoolEntry("Notify", "NotifyStatusChange"))
		{	
		b_notifyall->setEnabled(false);
		panebox->setEnabled(false);
		notifybox->setEnabled(false);
		soundbox->setEnabled(false);

		}

	QObject::connect(b_notifysound, SIGNAL(toggled(bool)), soundbox, SLOT(setEnabled(bool)));
	QObject::connect(b_notifyall, SIGNAL(toggled(bool)), this, SLOT(ifNotifyAll(bool)));
	QObject::connect(b_notifyglobal, SIGNAL(toggled(bool)), this, SLOT(ifNotifyGlobal(bool)));

}

void EventConfigSlots::onDestroyConfigDialog()
{
	kdebug("EventConfigSlots::onDestroyConfigDialog() \n");

#ifdef HAVE_OPENSSL
	QComboBox *cb_portselect=ConfigDialog::getComboBox("Network", "Default port to connect to servers");
	config_file.writeEntry("Network","DefaultPort",cb_portselect->currentText());
#endif
	QLineEdit *e_servers=ConfigDialog::getLineEdit("Network", "IP addresses:", "server");
	
	QStringList tmpservers,server;
	QValueList<QHostAddress> servers;
	QHostAddress ip;
	bool ipok;
	int i;
	
	    tmpservers = QStringList::split(";", e_servers->text());
		for (i = 0; i < tmpservers.count(); i++) 
		{
		ipok = ip.setAddress(tmpservers[i]);
			if (!ipok)
			    break;
			    servers.append(ip);
			    server.append(ip.toString());
		}
		config_file.writeEntry("Network","Server",server.join(";"));
		config_servers=servers;
			    server_nr = 0;
			    
	if (!config_dccip.setAddress(config_file.readEntry("Network","DccIP")))
	    {
		config_file.writeEntry("Network","DccIP","0.0.0.0");
		config_dccip.setAddress((unsigned int)0);
	    }										
	
	if (!config_extip.setAddress(config_file.readEntry("Network","ExternalIP")))
	    {	config_file.writeEntry("Network","ExternalIP","0.0.0.0");
		config_extip.setAddress((unsigned int)0);
	    }	
	
	if (config_file.readNumEntry("Network","ExternalPort")<=1023)
	    config_file.writeEntry("Network","ExternalPort",0);
	
	if (!ip.setAddress(config_file.readEntry("Network","ProxyHost")))
	    config_file.writeEntry("Network","ProxyHost","0.0.0.0");

	if (config_file.readNumEntry("Network","ProxyPort")<=1023)
	    config_file.writeEntry("Network","ProxyPort",0);


	//notify
	QListBox *e_availusers= ConfigDialog::getListBox("Notify", "available");
	QListBox *e_notifies= ConfigDialog::getListBox("Notify", "track");

	QString tmp;
    	for (i = 0; i < e_notifies->count(); i++) {
		tmp = e_notifies->text(i);
		userlist.byAltNick(tmp).notify = true;
		}
	for (i = 0; i < e_availusers->count(); i++) {
		tmp = e_availusers->text(i);
		userlist.byAltNick(tmp).notify = false;
		}

	/* and now, save it */
	userlist.writeToFile();	
	//


};


void EventConfigSlots::ifNotifyGlobal(bool toggled) {
	QCheckBox *b_notifyall= ConfigDialog::getCheckBox("Notify", "Notify about all users");
	QVGroupBox *notifybox= ConfigDialog::getVGroupBox("Notify", "Notify options");
	QGrid *panebox = ConfigDialog::getGrid("Notify","listboxy");
	QCheckBox *b_sounds= ConfigDialog::getCheckBox("Sounds", "Play sounds");
	QCheckBox *b_notifysound= ConfigDialog::getCheckBox("Notify", "Notify by sound");
	
	b_notifyall->setEnabled(toggled);
	panebox->setEnabled(toggled && !b_notifyall->isChecked());
	b_notifysound->setEnabled(toggled && b_sounds->isChecked());
	notifybox->setEnabled(toggled);
}

void EventConfigSlots::ifNotifyAll(bool toggled) {
	QGrid *panebox = ConfigDialog::getGrid("Notify","listboxy");
	panebox->setEnabled(!toggled);
}

void EventConfigSlots::_Left2( QListBoxItem *item) {
    _Left();
}

void EventConfigSlots::_Right2( QListBoxItem *item) {
    _Right();
}


void EventConfigSlots::_Left(void) {
	kdebug("EventConfigSlots::_Left()\n");
	QListBox *e_availusers= ConfigDialog::getListBox("Notify", "available");
	QListBox *e_notifies= ConfigDialog::getListBox("Notify", "track");

	if (e_notifies->currentItem() != -1) {
		e_availusers->insertItem(e_notifies->text(e_notifies->currentItem()));
		e_notifies->removeItem(e_notifies->currentItem());
		e_availusers->sort();
		}
}

void EventConfigSlots::_Right(void) {
	kdebug("EventConfigSlots::_Right()\n");
	QListBox *e_availusers= ConfigDialog::getListBox("Notify", "available");
	QListBox *e_notifies= ConfigDialog::getListBox("Notify", "track");

	if (e_availusers->currentItem() != -1) {
		e_notifies->insertItem(e_availusers->text(e_availusers->currentItem()));
		e_availusers->removeItem(e_availusers->currentItem());
		e_notifies->sort();
		}
}


void EventConfigSlots::chooseNotifyFile(void) {

	QLineEdit *e_soundnotify= ConfigDialog::getLineEdit("Notify", "Path:");
	QString s(QFileDialog::getOpenFileName( QString::null, "Audio Files (*.wav *.au *.raw)"));
	if (s.length())
		e_soundnotify->setText(s);
}


void EventConfigSlots::chooseNotifyTest(void) {
	QLineEdit *e_soundnotify= ConfigDialog::getLineEdit("Notify", "Path:");
	playSound(e_soundnotify->text(), config_file.readEntry("Sounds", "SoundPlayer"));
	
}


void EventConfigSlots::ifDccEnabled(bool value)
{
	kdebug("EventConfigSlots::ifDccEnabled() \n");

	QCheckBox *b_dccip= ConfigDialog::getCheckBox("Network", "DCC IP autodetection");
	QVGroupBox *g_dccip = ConfigDialog::getVGroupBox("Network", "DCC IP");
	QVGroupBox *g_fwdprop = ConfigDialog::getVGroupBox("Network", "DCC forwarding properties");
	QCheckBox *b_dccfwd = ConfigDialog::getCheckBox("Network", "DCC forwarding enabled");
	
	b_dccip->setEnabled(value);
	g_dccip->setEnabled(!b_dccip->isChecked()&& value);	
	b_dccfwd->setEnabled(value);
	g_fwdprop->setEnabled(b_dccfwd->isChecked() &&value);
};

void EventConfigSlots::ifDccIpEnabled(bool value)
{
	kdebug("EventConfigSlots::ifDccIpEnabled() \n");
	QVGroupBox *g_dccip = ConfigDialog::getVGroupBox("Network", "DCC IP");
	g_dccip->setEnabled(!value);
};

void EventConfigSlots::ifDefServerEnabled(bool value)
{
	kdebug("EventConfigSlots::ifDefServerEnabled() \n");
	QHBox *serverbox=(QHBox*)(ConfigDialog::getLineEdit("Network", "IP addresses:","server")->parent());
	serverbox->setEnabled(!value);	
};

void EventConfigSlots::useTlsEnabled(bool value)
{
#ifdef HAVE_OPENSSL
	kdebug("EventConfigSlots::useTlsEnabled() \n");
	QHBox *box_portselect=(QHBox*)(ConfigDialog::getComboBox("Network", "Default port to connect to servers")->parent());
	box_portselect->setEnabled(!value);
#endif
};


EventManager event_manager;
