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
//#include <iostream>
#include <klocale.h>
#include <qmessagebox.h>
#define _USE_BSD
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>

//
#include "kadu.h"
#include "events.h"
#include "pixmaps.h"
#include "chat.h"
#include "message.h"
#include "history.h"
#include "misc.h"
#include "vuser.h"
#include "pending_msgs.h"
#include "dock_widget.h"
#include "../config.h"
#ifdef HAVE_OPENSSL
extern "C"
{
#include "simlite.h"
};
#endif

SavePublicKey::SavePublicKey(uin_t uin, QString keyData, QWidget *parent, const char *name) :
	QDialog(parent, name, Qt::WDestructiveClose), uin(uin), keyData(keyData) {
	
	fprintf(stderr, "KK SavePublicKey::SavePublicKey()\n");

	setCaption(i18n("Save public key"));
	resize(200, 80);
	
//	QTextBrowser *qtext = new QTextBrowser(this);
//	qtext->setText(
//		i18n("User %1 is sending you his public key. Do you want to save it?").arg(userlist.byUin(uin).altnick));
//	qtext->setAlignment(Qt::AlignCenter);

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

	fprintf(stderr, "KK SavePublicKey::SavePublicKey(): finished\n");
}

void SavePublicKey::yesClicked() {
	QFile keyfile;
	QString keyfile_path;

	fprintf(stderr, "KK SavePublicKey::yesClicked()\n");

	keyfile_path.append(ggPath("keys/"));
	keyfile_path.append(QString::number(uin));
	keyfile_path.append(".pem");

	keyfile.setName(keyfile_path);

	if (!(keyfile.open(IO_WriteOnly))) {
		QMessageBox::critical(this, i18n("Error"), i18n("Error writting the key"), i18n("OK"), QString::null, 0);
		fprintf(stderr, "eventRecvMsg(): Error opening key file %s\n", (const char *)keyfile_path.local8Bit());
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

	fprintf(stderr, "KK SavePublicKey::yesClicked(): finished\n");
}

void eventRecvMsg(int msgclass, UinsList senders, unsigned char * msg, time_t time,int formats_count=0,struct gg_msg_format * formats=NULL)
{
	int i;

	fprintf(stderr, "KK eventRecvMsg()\n");

	// ignorujemy, jesli nick na liscie ignorowanych
	// PYTANIE CZY IGNORUJEMY CALA KONFERENCJE
	// JESLI PIERWSZY SENDER JEST IGNOROWANY????
	if (isIgnored(senders[0]))
		return;

	// ignorujemy wiadomosci systemowe (tylko na konsole)
	if (senders[0] == 0)
	{
		if (msgclass <= config.sysmsgidx)
		{
			fprintf(stderr, "KK Already had this message, ignoring\n");
			return;
		}
		config.sysmsgidx = msgclass;
		fprintf(stderr, "KK System message index %d\n", msgclass);
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

//			if (QMessageBox::information(parent, i18n("Save public key"),
//				i18n("User %1 is sending you his public key. Do you want to save it?").arg(userlist.byUin(senders[0]).altnick),
//				i18n("Yes"), i18n("No"), QString::null, 0, 1) != 0)
//				return;
			SavePublicKey *spk = new SavePublicKey(senders[0], __c2q((const char *)msg), NULL);
			spk->show();
			return;

			keyfile_path.append(ggPath("keys/"));
			keyfile_path.append(QString::number(senders[0]));
			keyfile_path.append(".pem");

			keyfile.setName(keyfile_path);

			if(!(keyfile.open(IO_WriteOnly))) {
				QMessageBox::critical(kadu, "Kadu", i18n("Error writting the key"), i18n("OK"), QString::null, 0);
				fprintf(stderr, "eventRecvMsg(): Error opening key file %s\n", (const char *)keyfile_path.local8Bit());
				return;
			}

			QString key_data=__c2q((const char *)msg);
			keyfile.writeBlock(key_data.local8Bit(), key_data.length());

			keyfile.close();
			return;
		}
	};

	if(msg != NULL)
	{
		fprintf(stderr,"KK Decrypting encrypted message...\n");
		char* decoded = sim_message_decrypt(msg, senders[0]);
		fprintf(stderr,"KK Decrypted message is: %s\n",decoded);
		if (decoded != NULL)
			strcpy((char *)msg, decoded);
	};
#endif

	if (msg != NULL)
		cp_to_iso(msg);

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
			if (dw)
				userlist.addUser("", "", nick, nick, "", nick, GG_STATUS_NOT_AVAIL,
					false, false, true, "", "", true);
			else
				kadu->addUser("", "", nick, nick, "", nick, GG_STATUS_NOT_AVAIL,
					"", "", true);
			}
	if (config.logmessages && senders[0] != config.uin)
		appendHistory(senders, senders[0], msg, FALSE, time);

	//script.eventMsg(senders[0],msgclass,(char*)msg);

	i = 0;
	while (i < chats.count() && !chats[i].uins.equals(senders))
		i++;

	if ((msgclass == GG_CLASS_CHAT || msgclass == GG_CLASS_MSG) && i < chats.count()) {
		QString toadd;
		tmp = __c2q((const char *)msg);

		chats[i].ptr->checkPresence(senders, tmp, time, toadd);
		chats[i].ptr->alertNewMessage();

		return;
		}

	playSound((const char *)config.soundmsg);

//	fprintf(stderr, "KK eventRecvMsg(): New buffer size: %d\n",pending.size());

	if (senders[0] != config.uin)
		pending.addMsg(senders, __c2q((const char*)msg), msgclass, time);

	fprintf(stderr, "KK eventRecvMsg(): Message allocated to slot %d\n", i);
	fprintf(stderr, "KK eventRecvMsg(): Got message from %d (%s) saying \"%s\"\n",
			senders[0], (const char *)nick.local8Bit(), msg);
											  
	UserBox::all_refresh();
	dw->changeIcon();

	if (config.raise) {
		kadu->showNormal();
		kadu->setFocus();
		}

	if (config.showhint) {
		if (!tip)
			tip = new DockHint(0);
		if (msgclass == GG_CLASS_CHAT)
			tip->Show(i18n("Chat with: ")+nick.latin1());
		if (msgclass == GG_CLASS_MSG)
			tip->Show(i18n("Message from: ")+nick.latin1());
	}

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

void playSound(const char *sound) {
	if (!config.playsound || mute)
		return;

	QStringList args;
	if ((QString::compare(sound,NULL) == 0) || (QString::compare(sound, "") == 0)) {
		fprintf(stderr,"KK No sound file specified?\n");
		return;
		}
	if (config.playartsdsp) {
		args.append("artsdsp");
		args.append(config.soundprog);
		}
	else
		args.append(config.soundprog);
	if (config.soundvolctrl)
		args.append(QString("-v %1").arg(config.soundvol));
	args.append(sound);
	for (QStringList::Iterator it = args.begin(); it != args.end(); ++it ) {
       		fprintf(stderr, "KK playSound(): %s\n", (*it).latin1());
		}
	QProcess *sndprocess = new QProcess(args, kadu);
	sndprocess->start();
	delete sndprocess;
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
		fprintf(stderr, "KK Notify about user\n");

		if (config.notifydialog) {		
			// FIXME convert into a regular QMessageBox
			QString msg;
			msg = i18n("User %1 is available").arg(userlist.byUin(uin).altnick);
			QMessageBox::information(0, i18n("User notify"), msg);		
			}

		if (config.notifysound)
	    		playSound((const char *)config.soundnotify);

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
			fprintf(stderr, "KK eventGotUserlist(): buddy %d not in list. Damned server!\n", n->uin);
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
			cp_to_iso((unsigned char *)e->event.notify_descr.descr);
			user.description.append(__c2q(e->event.notify_descr.descr));
			}

		switch (n->status) {
			case GG_STATUS_AVAIL:
				fprintf(stderr, "KK eventGotUserlist(): User %d went online\n", n->uin);
				break;
			case GG_STATUS_BUSY:
				fprintf(stderr, "KK eventGotUserlist(): User %d went busy\n", n->uin);
				break;
			case GG_STATUS_NOT_AVAIL:
				if (user.status == GG_STATUS_NOT_AVAIL
					|| user.status == GG_STATUS_INVISIBLE2) {
		    			fprintf(stderr, "KK eventGotUserlist(): User %d went offline (probably invisible ;))\n", n->uin);
					user.time_to_death = 300;
					}
				else
					if (userlist.byUin(n->uin).status != GG_STATUS_NOT_AVAIL)
						fprintf(stderr, "KK eventGotUserlist(): User %d went offline\n", n->uin);
				break;
			case GG_STATUS_BLOCKED:
				fprintf(stderr, "KK eventGotUserlist(): User %d has blocked us\n", n->uin);
				break;
			case GG_STATUS_BUSY_DESCR:
				fprintf(stderr, "KK eventGotUserlist(): User %d went busy with descr.\n", n->uin);
				break;
			case GG_STATUS_NOT_AVAIL_DESCR:
				fprintf(stderr, "KK eventGotUserlist(): User %d went offline with descr.\n", n->uin);
				break;
			case GG_STATUS_AVAIL_DESCR:
				fprintf(stderr, "KK eventGotUserlist(): User %d went online with descr.\n", n->uin);
				break;
			case GG_STATUS_INVISIBLE_DESCR:
				fprintf(stderr, "KK eventGotUserlist(): User %d went invisible with descr.\n", n->uin);
				break;
			default:
				fprintf(stderr, "KK eventGotUserlist(): Unknown status for user %d: %d\n", n->uin, n->status);
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

		for (i = 0; i < chats.count(); i++)
			if (chats[i].uins.contains(n->uin))
				chats[i].ptr->setTitle();

		ifNotify(n->uin, n->status, oldstatus);

		n++;		
		}

}

void eventStatusChange(struct gg_event * e) {
	fprintf(stderr, "KK eventStatusChange(): User %d went %d\n", e->event.status.uin,  e->event.status.status);

	unsigned int oldstatus;
	int i;
	
	UserListElement &user = userlist.byUin(e->event.status.uin);

	if (!userlist.containsUin(e->event.status.uin)) {
		// ignore!
		fprintf(stderr, "KK eventStatusChange(): buddy %d not in list. Damned server!\n", e->event.status.uin);
		gg_remove_notify(sess, e->event.status.uin);
		return;
		}

	oldstatus = user.status;

	if (user.description)
		user.description.truncate(0);
	if (ifStatusWithDescription(e->event.status.status)) {
		cp_to_iso((unsigned char *)e->event.status.descr);
		user.description.append(__c2q(e->event.status.descr));
		}
	userlist.changeUserStatus(e->event.status.uin, e->event.status.status);
	
	if (user.status == GG_STATUS_NOT_AVAIL || user.status == GG_STATUS_NOT_AVAIL_DESCR) {
		user.ip = 0;
		user.port = 0;
		}

	for (i = 0; i < chats.count(); i++)
		if (chats[i].uins.contains(e->event.status.uin))
			chats[i].ptr->setTitle();

	ifNotify(e->event.status.uin, e->event.status.status, oldstatus);
}

void kadu_debug(int debuglevel, char * message) {
//		fprintf(stderr, "Debug type %i: %s\n", debuglevel, message);
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

#include "events.moc"
