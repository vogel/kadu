/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <signal.h>
#include <qwidget.h>
#include <qdialog.h>
#include <qmultilineedit.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlistbox.h>
#include <qpixmap.h>
#include <qcombobox.h>
#include <qstring.h>
#include <qfile.h>
#include <qarray.h>
#include <qlayout.h>
#include <qtextbrowser.h>
#include <qdatetime.h>
#include <qmessagebox.h>
#include <qprocess.h>

#define _USE_BSD
#include <sys/time.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>

#include "kadu.h"
#include "config_dialog.h"
#include "events.h"
#include "pixmaps.h"
#include "chat.h"
#include "message.h"
#include "history.h"
#include "misc.h"
#include "pending_msgs.h"
#include "dock_widget.h"
#include "debug.h"
#include "sound.h"
#include "../config.h"
#ifdef HAVE_OPENSSL
extern "C"
{
#include "simlite.h"
};
#endif

QTime lastsoundtime;

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

void eventRecvMsg(int msgclass, UinsList senders, unsigned char *msg, time_t time,int formats_count,struct gg_msg_format * formats)
{
	int i;

	kdebug("eventRecvMsg()\n");

	// ignorujemy, jesli nick na liscie ignorowanych
	// PYTANIE CZY IGNORUJEMY CALA KONFERENCJE
	// JESLI PIERWSZY SENDER JEST IGNOROWANY????
	if (isIgnored(senders[0]))
		return;

	QString mesg = cp2unicode(msg);
	// ignorujemy wiadomosci systemowe (tylko na konsole)
	if (senders[0] == 0)
	{
		if (msgclass <= config.sysmsgidx)
		{
			kdebug("Already had this message, ignoring\n");
			return;
		}
		config.sysmsgidx = msgclass;
		kdebug("System message index %d\n", msgclass);
		return;
		//senders[0] = config.uin;
	}

	QString tmp;

#ifdef HAVE_OPENSSL
	if (config.encryption) {
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

	QString nick;
	if (userlist.containsUin(senders[0])) {
		UserListElement &ule = userlist.byUin(senders[0]);
		if (ule.anonymous && config.ignoreanonusers)
			return;
		nick = ule.altnick;
		}
	else
		if (senders[0] != config.uin) {
			if (config.ignoreanonusers)
				return;
			nick = QString::number(senders[0]);
			if (trayicon)
				userlist.addUser("", "", nick, nick, "", nick, GG_STATUS_NOT_AVAIL,
					false, false, true, "", "", true);
			else
				kadu->addUser("", "", nick, nick, "", nick, GG_STATUS_NOT_AVAIL,
					"", "", true);
			}
	if (config.logmessages && senders[0] != config.uin)
		history.appendMessage(senders, senders[0], mesg, FALSE, time);

	//script.eventMsg(senders[0],msgclass,(char*)msg);

	i = 0;
	while (i < chats.count() && !chats[i].uins.equals(senders))
		i++;

	if (((msgclass & GG_CLASS_CHAT) == GG_CLASS_CHAT || (msgclass & GG_CLASS_MSG) == GG_CLASS_MSG) && i < chats.count()) {
		QString toadd;

		chats[i].ptr->checkPresence(senders, mesg, time, toadd);
		chats[i].ptr->alertNewMessage();
		if (!chats[i].ptr->isActiveWindow() && config.hintalert)
			trayicon->showHint(i18n("New message from: "), nick,0);
		return;
		}

	playSound(config.soundmsg);

	if (senders[0] != config.uin)
		pending.addMsg(senders, mesg, msgclass, time);

	kdebug("eventRecvMsg(): Message allocated to slot %d\n", i);
	kdebug("eventRecvMsg(): Got message from %d (%s) saying \"%s\"\n",
			senders[0], (const char *)nick.local8Bit(), (const char *)mesg.local8Bit());
											  
	UserBox::all_refresh();
	trayicon->changeIcon();

	if (config.raise) {
		kadu->showNormal();
		kadu->setFocus();
		}

	if (msgclass == GG_CLASS_CHAT)
		trayicon->showHint(i18n("Chat with: "), nick,0);
	if (msgclass == GG_CLASS_MSG)
		trayicon->showHint(i18n("Message from: "), nick,0);

	PendingMsgs::Element elem;
/*
	if (senders[0] == config.uin) {
		rMessage *rmsg;
		elem = pending[i];
		rmsg = new rMessage("System", elem.msgclass, elem.uins, elem.msg);
		rmsg->init();
		rmsg->show();
		}*/
}

void ifNotify(uin_t uin, unsigned int status, unsigned int oldstatus)
{
	if (userlist.containsUin(uin)) {
		UserListElement ule = userlist.byUin(uin);
		if (!ule.notify && !config.notifyall)
			return;
		}
	else
		if (!config.notifyall)
			return;

	if (config.notifyglobal && (status == GG_STATUS_AVAIL ||
		status == GG_STATUS_AVAIL_DESCR || status == GG_STATUS_BUSY || status == GG_STATUS_BUSY_DESCR
		|| status == GG_STATUS_BLOCKED) &&
		(oldstatus == GG_STATUS_NOT_AVAIL || oldstatus == GG_STATUS_NOT_AVAIL_DESCR || oldstatus == GG_STATUS_INVISIBLE ||
		oldstatus == GG_STATUS_INVISIBLE_DESCR || oldstatus == GG_STATUS_INVISIBLE2)) {
		kdebug("Notify about user\n");

		if (config.notifydialog) {		
			// FIXME convert into a regular QMessageBox
			QString msg;
			msg = i18n("User %1 is available").arg(userlist.byUin(uin).altnick);
			QMessageBox *msgbox;
			msgbox = new QMessageBox(i18n("User notify"), msg, QMessageBox::NoIcon,
				QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton,
				0, 0, FALSE, Qt::WStyle_DialogBorder || Qt::WDestructiveClose);
			msgbox->show();
			}

		if (config.notifysound) {
			if (lastsoundtime.elapsed() >= 500)
				playSound(config.soundnotify);
			lastsoundtime.restart();
			}
		
		if (config.notifyhint)
			trayicon->showHint(i18n(" is available"),userlist.byUin(uin).altnick,1);
		}
}

void eventGotUserlist(struct gg_event *e) {
	struct gg_notify_reply *n;
	unsigned int oldstatus;
	int i;

	if (e->type == GG_EVENT_NOTIFY)
		n = e->event.notify;
	else
		n = e->event.notify_descr.notify;

	while (n->uin) {
		UserListElement &user = userlist.byUin(n->uin);

		if (!userlist.containsUin(n->uin)) {
			kdebug("eventGotUserlist(): buddy %d not in list. Damned server!\n", n->uin);
			gg_remove_notify(sess, n->uin);
			n++;
			continue;
			}

		user.ip = n->remote_ip;
		user.port = n->remote_port;

		oldstatus = user.status;

		if (user.description)
			user.description.truncate(0);

		if (e->type == GG_EVENT_NOTIFY_DESCR) {
			user.description.append(cp2unicode((unsigned char *)e->event.notify_descr.descr));
			}

		switch (n->status) {
			case GG_STATUS_AVAIL:
				kdebug("eventGotUserlist(): User %d went online\n", n->uin);
				break;
			case GG_STATUS_BUSY:
				kdebug("eventGotUserlist(): User %d went busy\n", n->uin);
				break;
			case GG_STATUS_NOT_AVAIL:
				if (user.status == GG_STATUS_NOT_AVAIL
					|| user.status == GG_STATUS_INVISIBLE2) {
		    			kdebug("eventGotUserlist(): User %d went offline (probably invisible ;))\n", n->uin);
					user.time_to_death = 300;
					}
				else
					if (userlist.byUin(n->uin).status != GG_STATUS_NOT_AVAIL)
						kdebug("eventGotUserlist(): User %d went offline\n", n->uin);
				break;
			case GG_STATUS_BLOCKED:
				kdebug("eventGotUserlist(): User %d has blocked us\n", n->uin);
				break;
			case GG_STATUS_BUSY_DESCR:
				kdebug("eventGotUserlist(): User %d went busy with descr.\n", n->uin);
				break;
			case GG_STATUS_NOT_AVAIL_DESCR:
				kdebug("eventGotUserlist(): User %d went offline with descr.\n", n->uin);
				break;
			case GG_STATUS_AVAIL_DESCR:
				kdebug("eventGotUserlist(): User %d went online with descr.\n", n->uin);
				break;
			case GG_STATUS_INVISIBLE_DESCR:
				kdebug("eventGotUserlist(): User %d went invisible with descr.\n", n->uin);
				break;
			default:
				kdebug("eventGotUserlist(): Unknown status for user %d: %d\n", n->uin, n->status);
				break;
			}
		if (n->status != GG_STATUS_NOT_AVAIL)
//			user.status = n->status;
			userlist.changeUserStatus(n->uin, n->status);
		else
			if (user.status == GG_STATUS_NOT_AVAIL
				|| user.status == GG_STATUS_INVISIBLE2)
//				user.status = GG_STATUS_INVISIBLE2;
				userlist.changeUserStatus(n->uin, GG_STATUS_INVISIBLE2);
			else
//				user.status = GG_STATUS_NOT_AVAIL;
				userlist.changeUserStatus(n->uin, GG_STATUS_NOT_AVAIL);

		history.appendStatus(user.uin, user.status, user.description.length() ? user.description : QString::null);

		for (i = 0; i < chats.count(); i++)
			if (chats[i].uins.contains(n->uin))
				chats[i].ptr->setTitle();

		ifNotify(n->uin, n->status, oldstatus);

		n++;		
		}

}

void eventStatusChange(struct gg_event * e) {
	kdebug("eventStatusChange(): User %d went %d\n", e->event.status.uin,  e->event.status.status);

	unsigned int oldstatus;
	int i;
	
	UserListElement &user = userlist.byUin(e->event.status.uin);

	if (!userlist.containsUin(e->event.status.uin)) {
		// ignore!
		kdebug("eventStatusChange(): buddy %d not in list. Damned server!\n", e->event.status.uin);
		gg_remove_notify(sess, e->event.status.uin);
		return;
		}

	oldstatus = user.status;

	if (user.description)
		user.description.truncate(0);
	if (ifStatusWithDescription(e->event.status.status)) {
		user.description.append(cp2unicode((unsigned char *)e->event.status.descr));
		}
	userlist.changeUserStatus(e->event.status.uin, e->event.status.status);
	
	if (user.status == GG_STATUS_NOT_AVAIL || user.status == GG_STATUS_NOT_AVAIL_DESCR) {
		user.ip = 0;
		user.port = 0;
		}

	history.appendStatus(user.uin, user.status, user.description.length() ? user.description : QString::null);

	for (i = 0; i < chats.count(); i++)
		if (chats[i].uins.contains(e->event.status.uin))
			chats[i].ptr->setTitle();

	ifNotify(e->event.status.uin, e->event.status.status, oldstatus);
}

void ackHandler(int seq) {
	int i,j,k;
	for (i = 0; i < acks.size(); i++)
		if (acks[i].seq == seq && acks[i].ptr)
			if (acks[i].type < 2)
				((Message *)acks[i].ptr)->gotAck();
			else {
				acks[i].ack--;
				j = 0;
				while (j < chats.count() && chats[j].ptr != acks[i].ptr)
					j++;		
				if (j < chats.size() && !acks[i].ack)
					((Chat *)acks[i].ptr)->writeMyMessage();
				if (j == chats.count() || !acks[i].ack) {
					for (k = i + 1; k < acks.size(); k++) {
						acks[i-1].seq = acks[i].seq;
						acks[i-1].type = acks[i].type;
						acks[i-1].ptr = acks[i].ptr;
						}
					acks.resize(acks.size() - 1);
					}
				}
}

