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
#include <qobject.h>

#include <sys/types.h>
#include <sys/socket.h>
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
#include "debug.h"
#include "dcc.h"
#include "config_dialog.h"
#include "config_file.h"
#include "gadu.h"
#include "status.h"

AutoConnectionTimer *AutoConnectionTimer::autoconnection_object = NULL;
ConnectionTimeoutTimer *ConnectionTimeoutTimer::connectiontimeout_object = NULL;

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
	start(5000, TRUE);
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

EventManager::EventManager()
{
	connect(this,SIGNAL(userStatusChanged(struct gg_event*)),this,SLOT(userStatusChangedSlot(struct gg_event*)));
	connect(this,SIGNAL(userlistReceived(struct gg_event*)),this,SLOT(userlistReceivedSlot(struct gg_event*)));
	connect(this,SIGNAL(messageReceived(int,UinsList,QCString&,time_t, QByteArray&)),this,SLOT(messageReceivedSlot(int,UinsList,QCString&,time_t, QByteArray&)));
	connect(this,SIGNAL(systemMessageReceived(QString &, QDateTime &, int, void *)),
		this, SLOT(systemMessageReceivedSlot(QString &, QDateTime &, int, void *)));
	connect(this,SIGNAL(chatMsgReceived2(UinsList,const QString&,time_t)),
		this,SLOT(chatMsgReceived2Slot(UinsList,const QString&,time_t)));
	connect(this,SIGNAL(imageRequestReceived(UinType,uint32_t,uint32_t)),
		this,SLOT(imageRequestReceivedSlot(UinType,uint32_t,uint32_t)));
	connect(this,SIGNAL(imageReceived(UinType,uint32_t,uint32_t,const QString&,const char*)),
		this,SLOT(imageReceivedSlot(UinType,uint32_t,uint32_t,const QString&,const char*)));
	connect(this, SIGNAL(imageReceivedAndSaved(UinType,uint32_t,uint32_t,const QString&)),
		this, SLOT(imageReceivedAndSavedSlot(UinType,uint32_t,uint32_t,const QString&)));
	connect(this,SIGNAL(ackReceived(int)),this,SLOT(ackReceivedSlot(int)));
	connect(this,SIGNAL(dccConnectionReceived(const UserListElement&)),
		this,SLOT(dccConnectionReceivedSlot(const UserListElement&)));
}

void EventManager::systemMessageReceivedSlot(QString &msg, QDateTime &time,
	int formats_length, void *formats)
{
	kdebugf();
	MessageBox::msg(time.toString("hh:mm:ss (dd.MM.yyyy): ") + msg);
}

void EventManager::messageReceivedSlot(int msgclass, UinsList senders,QCString& msg, time_t time,
	QByteArray& formats)
{
/*
	sprawdzamy czy user jest na naszej liscie, jezeli nie to .anonymous zwroci true
	i czy jest wlaczona opcja ignorowania nieznajomych
	jezeli warunek jest spelniony przerywamy dzialanie funkcji.
*/
	if (userlist.byUinValue(senders[0]).anonymous && config_file.readBoolEntry("Chat","IgnoreAnonymousUsers")) {
		kdebugm(KDEBUG_INFO, "EventManager::messageReceivedSlot(): Ignored anonymous. %d is ignored\n",senders[0]);
		return;
		}

	// ignorujemy, jesli nick na liscie ignorowanych
	// PYTANIE CZY IGNORUJEMY CALA KONFERENCJE
	// JESLI PIERWSZY SENDER JEST IGNOROWANY????
	if (isIgnored(senders))
		return;

	bool block = false;
	emit messageFiltering(senders,msg,formats,block);
	if(block)
		return;

	const char* msg_c = msg;
	QString mesg = cp2unicode((const unsigned char*)msg_c);
	QDateTime datetime;
	datetime.setTime_t(time);

	bool grab=false;
	emit chatMsgReceived0(senders,mesg,time,grab);
	if (grab)
		return;

	// wiadomosci systemowe maja sensers[0] = 0
	// FIX ME!!!
	if (senders[0] == 0) {
		if (msgclass <= config_file.readNumEntry("General", "SystemMsgIndex", 0)) {
			kdebugm(KDEBUG_INFO, "Already had this message, ignoring\n");
			return;
			}
		config_file.writeEntry("General", "SystemMsgIndex", msgclass);
		kdebugm(KDEBUG_INFO, "System message index %d\n", msgclass);
		
		emit systemMessageReceived(mesg, datetime, formats.size(), formats.data());
		return;
		}

	mesg = formatGGMessage(mesg, formats.size(), formats.data(), senders[0]);

	if(!userlist.containsUin(senders[0]))
		userlist.addAnonymous(senders[0]);

	kdebugm(KDEBUG_INFO, "eventRecvMsg(): Got message from %d saying \"%s\"\n",
			senders[0], (const char *)mesg.local8Bit());

	emit chatMsgReceived1(senders,mesg,time,grab);
	if(!grab)
		emit chatMsgReceived2(senders,mesg,time);
}

void EventManager::chatMsgReceived2Slot(UinsList senders,const QString& msg,time_t time)
{
//	UserListElement ule = userlist.byUinValue(senders[0]);		

	pending.addMsg(senders, msg, GG_CLASS_CHAT, time);
	
	UserBox::all_refresh();

	if (config_file.readBoolEntry("General","AutoRaise")) {
		kadu->showNormal();
		kadu->setFocus();
		}

	hintmanager->addHintNewChat(senders, msg);

	if(config_file.readBoolEntry("Chat","OpenChatOnMessage"))
		pending.openMessages();
}

void EventManager::imageRequestReceivedSlot(UinType sender,uint32_t size,uint32_t crc32)
{
	kdebugm(KDEBUG_INFO, QString("Received image request. sender: %1, size: %2, crc32: %3\n").arg(sender).arg(size).arg(crc32).local8Bit().data());
	gadu_images_manager.sendImage(sender,size,crc32);
}	

void EventManager::imageReceivedSlot(UinType sender,uint32_t size,uint32_t crc32,const QString& filename,const char* data)
{
	kdebugm(KDEBUG_INFO, QString("Received image. sender: %1, size: %2, crc32: %3,filename: %4\n").arg(sender).arg(size).arg(crc32).arg(filename).local8Bit().data());
	QString full_path = gadu_images_manager.saveImage(sender,size,crc32,filename,data);
	emit imageReceivedAndSaved(sender,size,crc32,full_path);
}	

void EventManager::imageReceivedAndSavedSlot(UinType sender,uint32_t size,uint32_t crc32,const QString& path)
{
	for (int i = 0; i < pending.count(); i++)
	{
		PendingMsgs::Element& e = pending[i];
		e.msg = gadu_images_manager.replaceLoadingImages(e.msg,sender,size,crc32);
	}
}

void ifNotify(UinType uin, unsigned int status, unsigned int oldstatus)
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
		kdebugm(KDEBUG_INFO, "Notify about user\n");

		if (config_file.readBoolEntry("Notify","NotifyWithDialogBox")) {
			// FIXME convert into a regular QMessageBox
			QString msg;
			msg = QString(QT_TR_NOOP("User %1 is available")).arg(userlist.byUin(uin).altnick);
			QMessageBox *msgbox;
			msgbox = new QMessageBox(qApp->translate("@default",QT_TR_NOOP("User notify")), qApp->translate("@default",msg), QMessageBox::NoIcon,
				QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton,
				0, 0, FALSE, Qt::WStyle_DialogBorder || Qt::WDestructiveClose);
			msgbox->show();
			}

			}
}

void EventManager::userlistReceivedSlot(struct gg_event *e) {
	unsigned int oldstatus;
	int nr = 0;

	while (e->event.notify60[nr].uin) {
		UserListElement &user = userlist.byUin(e->event.notify60[nr].uin);

		if (!userlist.containsUin(e->event.notify60[nr].uin)) {
			kdebugm(KDEBUG_INFO, "eventGotUserlist(): buddy %d not in list. Damned server!\n",
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
				kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "eventGotUserlist(): User %d went online\n",
					e->event.notify60[nr].uin);
				break;
			case GG_STATUS_BUSY:
				kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "eventGotUserlist(): User %d went busy\n",
					e->event.notify60[nr].uin);
				break;
			case GG_STATUS_NOT_AVAIL:
				kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "eventGotUserlist(): User %d went offline\n",
					e->event.notify60[nr].uin);
				break;
			case GG_STATUS_BLOCKED:
				kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "eventGotUserlist(): User %d has blocked us\n",
					e->event.notify60[nr].uin);
				break;
			case GG_STATUS_BUSY_DESCR:
				kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "eventGotUserlist(): User %d went busy with descr.\n",
					e->event.notify60[nr].uin);
				break;
			case GG_STATUS_NOT_AVAIL_DESCR:
				kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "eventGotUserlist(): User %d went offline with descr.\n",
					e->event.notify60[nr].uin);
				break;
			case GG_STATUS_AVAIL_DESCR:
				kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "eventGotUserlist(): User %d went online with descr.\n",
					e->event.notify60[nr].uin);
				break;
			case GG_STATUS_INVISIBLE_DESCR:
				kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "eventGotUserlist(): User %d went invisible with descr.\n",
					e->event.notify60[nr].uin);
				break;
			default:
				kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "eventGotUserlist(): Unknown status for user %d: %d\n",
					e->event.notify60[nr].uin, e->event.notify60[nr].status);
				break;
			}
		userlist.changeUserStatus(e->event.notify60[nr].uin, e->event.notify60[nr].status);

		history.appendStatus(user.uin, user.status, user.description.length() ? user.description : QString::null);

		chat_manager->refreshTitlesForUin(e->event.notify60[nr].uin);

		ifNotify(e->event.notify60[nr].uin, e->event.notify60[nr].status, oldstatus);

		nr++;		
		}
	UserBox::all_refresh();
}

void EventManager::userStatusChangedSlot(struct gg_event * e) {
	unsigned int oldstatus, status;
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

	kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "eventStatusChange(): User %d went %d\n", uin, status);
	UserListElement &user = userlist.byUin(uin);

	if (!userlist.containsUin(uin)) {
		// ignore!
		kdebugm(KDEBUG_INFO, "eventStatusChange(): buddy %d not in list. Damned server!\n", uin);
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

	chat_manager->refreshTitlesForUin(uin);
			
	ifNotify(uin, status, oldstatus);
	UserBox::all_refresh();
}

void EventManager::ackReceivedSlot(int seq)
{
	kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "EventManager::ackReceivedSlot(): got msg ack.\n");
}

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
}

void EventManager::eventHandler(gg_session* sess)
{
	static int calls = 0;

	kdebugf();
	calls++;
	if (calls > 1)
		kdebugm(KDEBUG_WARNING, "************* EventManager::eventHandler(): Recursive eventHandler calls detected!\n");

	gg_event* e;
	if (!(e = gg_watch_fd(sess))) {
		emit connectionBroken();
		gg_free_event(e);
		calls--;
		return;
		}

	if (sess->state == GG_STATE_CONNECTING_HUB || sess->state == GG_STATE_CONNECTING_GG)
	{
		kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "EventManager::eventHandler(): changing QSocketNotifiers.\n");

		kadusnw->setEnabled(false);
		delete kadusnw;

		kadusnr->setEnabled(false);
		delete kadusnr;

		kadusnw = new QSocketNotifier(sess->fd, QSocketNotifier::Write, this); 
		QObject::connect(kadusnw, SIGNAL(activated(int)), kadu, SLOT(dataSent()));

		kadusnr = new QSocketNotifier(sess->fd, QSocketNotifier::Read, this); 
		QObject::connect(kadusnr, SIGNAL(activated(int)), kadu, SLOT(dataReceived()));
	}

	switch (sess->state)
	{
		case GG_STATE_RESOLVING:
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "EventManager::eventHandler(): Resolving address\n");
			break;
		case GG_STATE_CONNECTING_HUB:
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "EventManager::eventHandler(): Connecting to hub\n");
			break;
		case GG_STATE_READING_DATA:
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "EventManager::eventHandler(): Fetching data from hub\n");
			break;
		case GG_STATE_CONNECTING_GG:
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "EventManager::eventHandler(): Connecting to server\n");
			break;
		case GG_STATE_READING_KEY:
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "EventManager::eventHandler(): Waiting for hash key\n");
			ConnectionTimeoutTimer::off();
			break;
		case GG_STATE_READING_REPLY:
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "EventManager::eventHandler(): Sending key\n");
			ConnectionTimeoutTimer::off();
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
		UinsList uins;
		if (e->event.msg.msgclass == GG_CLASS_CTCP) {
			uins.append(e->event.msg.sender);
			if (config_file.readBoolEntry("Network", "AllowDCC") && !isIgnored(uins))
				emit dccConnectionReceived(userlist.byUin(e->event.msg.sender));
			}
		else {
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "eventHandler(): %d\n", e->event.msg.recipients_count);
			if ((e->event.msg.msgclass & GG_CLASS_CHAT) == GG_CLASS_CHAT) {
				uins.append(e->event.msg.sender);
				for (int i = 0; i < e->event.msg.recipients_count; i++)
					uins.append(e->event.msg.recipients[i]);
				}
			else
				uins.append(e->event.msg.sender);
			QCString msg((char*)e->event.msg.message);
			QByteArray formats;
			formats.duplicate((const char*)e->event.msg.formats, e->event.msg.formats_length);
			emit messageReceived(e->event.msg.msgclass, uins, msg,
				e->event.msg.time, formats);
			}
		}

	if (e->type == GG_EVENT_IMAGE_REQUEST)
	{
		kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "Image request received\n");
		emit imageRequestReceived(
			e->event.image_request.sender,
			e->event.image_request.size,
			e->event.image_request.crc32);
	}

	if (e->type == GG_EVENT_IMAGE_REPLY)
	{
		kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "Image reply received\n");
		emit imageReceived(
			e->event.image_reply.sender,
			e->event.image_reply.size,
			e->event.image_reply.crc32,
			e->event.image_reply.filename,
			e->event.image_reply.image);
	}

	if (e->type == GG_EVENT_STATUS60 || e->type == GG_EVENT_STATUS)
		emit userStatusChanged(e);

	if (e->type == GG_EVENT_ACK) {
		kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "EventManager::eventHandler(): message reached %d (seq %d)\n",
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
}

void EventConfigSlots::initModule()
{
	kdebugf();
	
	EventConfigSlots *eventconfigslots = new EventConfigSlots();

// zakladka "powiadom"
	ConfigDialog::addTab(QT_TRANSLATE_NOOP("@default", "Notify"));
	ConfigDialog::addCheckBox("Notify", "Notify", QT_TRANSLATE_NOOP("@default", "Notify when users become available"), "NotifyStatusChange", false);
	ConfigDialog::addCheckBox("Notify", "Notify", QT_TRANSLATE_NOOP("@default", "Notify about all users"), "NotifyAboutAll", false);
	ConfigDialog::addGrid("Notify", "Notify" ,"listboxy",3);
	
	ConfigDialog::addGrid("Notify", "listboxy", "listbox1", 1);
	ConfigDialog::addLabel("Notify", "listbox1", QT_TRANSLATE_NOOP("@default", "Available"));
	ConfigDialog::addListBox("Notify", "listbox1","available");
	
	ConfigDialog::addGrid("Notify", "listboxy", "listbox2", 1);
	ConfigDialog::addPushButton("Notify", "listbox2", "", "AddToNotifyList","","forward");
	ConfigDialog::addPushButton("Notify", "listbox2", "", "RemoveFromNotifyList","","back");
	
	ConfigDialog::addGrid("Notify", "listboxy", "listbox3", 1);
	ConfigDialog::addLabel("Notify", "listbox3", QT_TRANSLATE_NOOP("@default", "Tracked"));
	ConfigDialog::addListBox("Notify", "listbox3", "track");
	
	ConfigDialog::addVGroupBox("Notify", "Notify", QT_TRANSLATE_NOOP("@default", "Notify options"));
	ConfigDialog::addCheckBox("Notify", "Notify options", QT_TRANSLATE_NOOP("@default", "Notify by dialog box"), "NotifyWithDialogBox", false);

//zakladka "siec"
	ConfigDialog::addTab(QT_TRANSLATE_NOOP("@default", "Network"));
	ConfigDialog::addCheckBox("Network", "Network", QT_TRANSLATE_NOOP("@default", "DCC enabled"), "AllowDCC", false);
	ConfigDialog::addCheckBox("Network", "Network", QT_TRANSLATE_NOOP("@default", "DCC IP autodetection"), "DccIpDetect", false);
	
	ConfigDialog::addVGroupBox("Network", "Network", QT_TRANSLATE_NOOP("@default", "DCC IP"));
	ConfigDialog::addLineEdit("Network", "DCC IP", QT_TRANSLATE_NOOP("@default", "IP address:"),"DccIP");
	ConfigDialog::addCheckBox("Network", "Network", QT_TRANSLATE_NOOP("@default", "DCC forwarding enabled"), "DccForwarding", false);
	
	ConfigDialog::addVGroupBox("Network", "Network", QT_TRANSLATE_NOOP("@default", "DCC forwarding properties"));
	ConfigDialog::addLineEdit("Network", "DCC forwarding properties", QT_TRANSLATE_NOOP("@default", "External IP address:"), "ExternalIP");
	ConfigDialog::addLineEdit("Network", "DCC forwarding properties", QT_TRANSLATE_NOOP("@default", "External TCP port:"), "ExternalPort", "0");
	ConfigDialog::addLineEdit("Network", "DCC forwarding properties", QT_TRANSLATE_NOOP("@default", "Local TCP port:"), "LocalPort", "1550");

	ConfigDialog::addVGroupBox("Network", "Network", QT_TRANSLATE_NOOP("@default", "Servers properties"));
	ConfigDialog::addGrid("Network", "Servers properties", "servergrid", 2);
	ConfigDialog::addCheckBox("Network", "servergrid", QT_TRANSLATE_NOOP("@default", "Use default servers"), "isDefServers", true);
#ifdef HAVE_OPENSSL
	ConfigDialog::addCheckBox("Network", "servergrid", QT_TRANSLATE_NOOP("@default", "Use TLSv1"), "UseTLS", false);
#endif
	ConfigDialog::addLineEdit("Network", "Servers properties", QT_TRANSLATE_NOOP("@default", "IP addresses:"), "Server","","","server");
#ifdef HAVE_OPENSSL
	ConfigDialog::addComboBox("Network", "Servers properties", QT_TRANSLATE_NOOP("@default", "Default port to connect to servers"));
#endif
	ConfigDialog::addCheckBox("Network", "Network", QT_TRANSLATE_NOOP("@default", "Use proxy server"), "UseProxy", false);

	ConfigDialog::addVGroupBox("Network", "Network", QT_TRANSLATE_NOOP("@default", "Proxy server"));
	ConfigDialog::addGrid("Network", "Proxy server", "proxygrid", 2);
	ConfigDialog::addLineEdit("Network", "proxygrid", "IP address:", "ProxyHost", "","","proxyhost");
	ConfigDialog::addLineEdit("Network", "proxygrid", QT_TRANSLATE_NOOP("@default", "Port:"), "ProxyPort", "0");
	ConfigDialog::addLineEdit("Network", "proxygrid", QT_TRANSLATE_NOOP("@default", "Username:"), "ProxyUser");
	ConfigDialog::addLineEdit("Network", "proxygrid", QT_TRANSLATE_NOOP("@default", "Password:"), "ProxyPassword");
	
	ConfigDialog::registerSlotOnCreate(eventconfigslots, SLOT(onCreateConfigDialog()));
	ConfigDialog::registerSlotOnApply(eventconfigslots, SLOT(onDestroyConfigDialog()));
	
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
	for (unsigned int i = 0; i < servers.count(); i++)
	{
		if (ip2.setAddress(servers[i]))
			config_servers.append(ip2);
	}
	server_nr = 0;


	ConfigDialog::connectSlot("Notify", "", SIGNAL(clicked()), eventconfigslots, SLOT(_Right()), "forward");
	ConfigDialog::connectSlot("Notify", "", SIGNAL(clicked()), eventconfigslots, SLOT(_Left()), "back");
	ConfigDialog::connectSlot("Notify", "available", SIGNAL(doubleClicked(QListBoxItem *)), eventconfigslots, SLOT(_Right2(QListBoxItem *)));
	ConfigDialog::connectSlot("Notify", "track", SIGNAL(doubleClicked(QListBoxItem *)), eventconfigslots, SLOT(_Left2(QListBoxItem *)));
}

void EventConfigSlots::onCreateConfigDialog()
{
	kdebugf();
	
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
	for (unsigned int i=0; i < userlist.count(); i++) {
		if (userlist[i].uin)
			if (!userlist[i].notify)
				e_availusers->insertItem(userlist[i].altnick);
			else
				e_notifies->insertItem(userlist[i].altnick);
		}

	e_availusers->sort();
	e_notifies->sort();
	e_availusers->setSelectionMode(QListBox::Extended);
	e_notifies->setSelectionMode(QListBox::Extended);

	QCheckBox *b_notifyglobal= ConfigDialog::getCheckBox("Notify", "Notify when users become available");
	QCheckBox *b_notifyall= ConfigDialog::getCheckBox("Notify", "Notify about all users");
	QVGroupBox *notifybox= ConfigDialog::getVGroupBox("Notify", "Notify options");
	QGrid *panebox = ConfigDialog::getGrid("Notify","listboxy");	
	
	if (config_file.readBoolEntry("Notify", "NotifyAboutAll")) 
		panebox->setEnabled(false);

	if (!config_file.readBoolEntry("Notify", "NotifyStatusChange"))
		{	
		b_notifyall->setEnabled(false);
		panebox->setEnabled(false);
		notifybox->setEnabled(false);
		}

	QObject::connect(b_notifyall, SIGNAL(toggled(bool)), this, SLOT(ifNotifyAll(bool)));
	QObject::connect(b_notifyglobal, SIGNAL(toggled(bool)), this, SLOT(ifNotifyGlobal(bool)));

}

void EventConfigSlots::onDestroyConfigDialog()
{
	kdebugf();

#ifdef HAVE_OPENSSL
	QComboBox *cb_portselect=ConfigDialog::getComboBox("Network", "Default port to connect to servers");
	config_file.writeEntry("Network","DefaultPort",cb_portselect->currentText());
#endif
	QLineEdit *e_servers=ConfigDialog::getLineEdit("Network", "IP addresses:", "server");
	
	QStringList tmpservers,server;
	QValueList<QHostAddress> servers;
	QHostAddress ip;
	bool ipok;
	unsigned int i;
	
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
	{
		config_file.writeEntry("Network","ExternalIP","0.0.0.0");
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
}

void EventConfigSlots::ifNotifyGlobal(bool toggled) {
	QCheckBox *b_notifyall= ConfigDialog::getCheckBox("Notify", "Notify about all users");
	QVGroupBox *notifybox= ConfigDialog::getVGroupBox("Notify", "Notify options");
	QGrid *panebox = ConfigDialog::getGrid("Notify","listboxy");
	
	b_notifyall->setEnabled(toggled);
	panebox->setEnabled(toggled && !b_notifyall->isChecked());
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
	kdebugf();
	QListBox *e_availusers= ConfigDialog::getListBox("Notify", "available");
	QListBox *e_notifies= ConfigDialog::getListBox("Notify", "track");
	QStringList tomove;
	unsigned int i;

	for(i=0;i<e_notifies->count();i++){
		if (e_notifies->isSelected(i))
			tomove+=e_notifies->text(i);
	}

	for(i=0;i<tomove.size();i++){
		e_availusers->insertItem(tomove[i]);
		e_notifies->removeItem(e_notifies->index(e_notifies->findItem(tomove[i])));
	}

	e_availusers->sort();
}

void EventConfigSlots::_Right(void) {
	kdebugf();
	QListBox *e_availusers= ConfigDialog::getListBox("Notify", "available");
	QListBox *e_notifies= ConfigDialog::getListBox("Notify", "track");
	QStringList tomove;
	unsigned int i;

	for(i=0;i<e_availusers->count();i++){
		if (e_availusers->isSelected(i))
			tomove+=e_availusers->text(i);
	}

	for(i=0;i<tomove.size();i++){
		e_notifies->insertItem(tomove[i]);
		e_availusers->removeItem(e_availusers->index(e_availusers->findItem(tomove[i])));
	}

	e_notifies->sort();
}


void EventConfigSlots::ifDccEnabled(bool value)
{
	kdebugf();

	QCheckBox *b_dccip= ConfigDialog::getCheckBox("Network", "DCC IP autodetection");
	QVGroupBox *g_dccip = ConfigDialog::getVGroupBox("Network", "DCC IP");
	QVGroupBox *g_fwdprop = ConfigDialog::getVGroupBox("Network", "DCC forwarding properties");
	QCheckBox *b_dccfwd = ConfigDialog::getCheckBox("Network", "DCC forwarding enabled");
	
	b_dccip->setEnabled(value);
	g_dccip->setEnabled(!b_dccip->isChecked()&& value);	
	b_dccfwd->setEnabled(value);
	g_fwdprop->setEnabled(b_dccfwd->isChecked() &&value);
}

void EventConfigSlots::ifDccIpEnabled(bool value)
{
	kdebugf();
	QVGroupBox *g_dccip = ConfigDialog::getVGroupBox("Network", "DCC IP");
	g_dccip->setEnabled(!value);
}

void EventConfigSlots::ifDefServerEnabled(bool value)
{
	kdebugf();
	QHBox *serverbox=(QHBox*)(ConfigDialog::getLineEdit("Network", "IP addresses:","server")->parent());
	serverbox->setEnabled(!value);	
}

void EventConfigSlots::useTlsEnabled(bool value)
{
#ifdef HAVE_OPENSSL
	kdebugf();
	QHBox *box_portselect=(QHBox*)(ConfigDialog::getComboBox("Network", "Default port to connect to servers")->parent());
	box_portselect->setEnabled(!value);
#endif
}


EventManager event_manager;
