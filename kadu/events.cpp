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
#include <iostream>
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
#include "sim.h"
#endif

void sigchldHndl (int whatever) {
	while ((wait3(NULL, WNOHANG, NULL)) > 0);
}

void eventRecvMsg(int msgclass, UinsList senders, unsigned char * msg, time_t time,int formats_count=0,struct gg_msg_format * formats=NULL) {
	QString tmp;
#ifdef HAVE_OPENSSL
	int declen = 0;
	char decoded[2048];

	if(msg != NULL) {
		int msglen = strlen((const char *)msg);
		memset(decoded, 0, 2048);
		declen = SIM_Message_Decrypt((unsigned char *)msg, (unsigned char *)decoded, msglen, senders[0]);
		fprintf(stderr, "DECLEN=%d\n", declen);
	}
#endif

	fprintf(stderr, "KK eventRecvMsg()\n");

	if (isIgnored(senders[0]))
		return;

	/* check whether it's a system message */
	if (senders[0] == 0) {
		if (msgclass <= config.sysmsgidx) {
			fprintf(stderr, "KK Already had this message, ignoring\n");
			return;
			}
		config.sysmsgidx = msgclass;
		fprintf(stderr, "KK System message index %d\n", msgclass);
		senders[0] = config.uin;
		} else if (msg != NULL)
#ifdef HAVE_OPENSSL
                       if (declen > 0)
                               cp_to_iso((unsigned char *)decoded);
                       else
#endif
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
//			kadu->addUser("", "", nick, nick, "", nick, GG_STATUS_NOT_AVAIL, "", "", true);
			userlist.addUser("", "", nick, nick, "", nick, GG_STATUS_NOT_AVAIL,
				false, false, true, "", "", true);
			}
	if (config.logmessages && senders[0] != config.uin)
#ifdef HAVE_OPENSSL
               if (declen > 0)
                       appendHistory(senders, senders[0], (unsigned char *)decoded, FALSE, time);
               else
#endif
                       appendHistory(senders, senders[0], msg, FALSE, time);

	//script.eventMsg(senders[0],msgclass,(char*)msg);

	int i;
	i = 0;
	while (i < chats.count() && !chats[i].uins.equals(senders))
		i++;

#ifdef HAVE_OPENSSL
	if (config.encryption) {
		if (!strncmp((char *)msg, "-----BEGIN RSA PUBLIC KEY-----", 20)) {
			QFile keyfile;
			QString keyfile_path;
			QString key_data;
			switch(QMessageBox::information(kadu, "Kadu", i18n("User") +  (const char *) + i18n("is sending you public key. Do you want to save them ?"), i18n("Yes"), i18n("No"), QString::null, 0, 1)) {
				case 0: // Yes ? ;)
					keyfile_path.append(ggPath("keys/"));
					keyfile_path.append(QString::number(senders[0]));
					keyfile_path.append(".pem");

					keyfile.setName(keyfile_path);

					if(!(keyfile.open(IO_WriteOnly))) {
						QMessageBox::critical(kadu, "Kadu", i18n("Nie mozna zapisa. klucza"), i18n("OK"), QString::null, 0);
						fprintf(stderr, "eventRecvMsg(): Error opening key file %s\n", (const char *)keyfile_path.local8Bit());					
						return;
					}

					key_data.append(__c2q((const char *)msg));

					keyfile.writeBlock(key_data.local8Bit(), key_data.length());

					keyfile.close();

					SIM_KC_Free(SIM_KC_Find(senders[0]));

					return;
				case 1: // No ? ;)
					return;
			}
		}
	}
#endif
	if ((msgclass == GG_CLASS_CHAT || msgclass == GG_CLASS_MSG) && i < chats.count()) {
		QString toadd;
#ifdef HAVE_OPENSSL
		if (declen > 0)
			tmp = __c2q((const char *)decoded);
		else
#endif
                        tmp = __c2q((const char *)msg);

		chats[i].ptr->checkPresence(senders, tmp, time, toadd);
		chats[i].ptr->alertNewMessage();

		return;
		}

	playSound(config.soundmsg);

//	fprintf(stderr, "KK eventRecvMsg(): New buffer size: %d\n",pending.size());

	if (senders[0] != config.uin)
#ifdef HAVE_OPENSSL
		if (declen > 0)
			pending.addMsg(senders, __c2q((const char*)decoded), msgclass, time);
		else
#endif
			pending.addMsg(senders, __c2q((const char*)msg), msgclass, time);

	fprintf(stderr, "KK eventRecvMsg(): Message allocated to slot %d\n", i);
#ifdef HAVE_OPENSSL
	if (declen > 0)
		fprintf(stderr, "KK eventRecvMsg(): Got encoded message from %d (%s) saying \"%s\"\n",
		         senders[0], (const char *)nick.local8Bit(), decoded);
        else
#endif
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

	if (senders[0] == config.uin) {
		rMessage *rmsg;
		elem = pending[i];
		rmsg = new rMessage("System", elem.msgclass, elem.uins, elem.msg);
		rmsg->init();
		rmsg->show();
		}
}

void playSound(char *sound) {
	if (!config.playsound || mute)
		return;

	QStringList args;
	if ((QString::compare(sound,NULL) == 0) || (QString::compare(sound, "") == 0)) {
		fprintf(stderr,"KK No sound file specified?\n");
		return;
		}
	if (config.playartsdsp) {
		args.append("artsdsp");
		if (config.soundvolctrl) {
//			args.append(QString("artsdsp %1 -v %.2f %3").arg(config.soundprog).arg(config.soundvol).arg(sound));
			args.append(config.soundprog);
			args.append(QString("-v %1").arg(config.soundvol));
			}
		else
			args.append(config.soundprog);
		}
	else {
		if (config.soundvolctrl) {
//			args.append(QString("%s -v %.2f %s", config.soundprog, config.soundvol, sound);
			args.append(config.soundprog);
			args.append(QString("-v %1").arg(config.soundvol));
			}
		else
			args.append(config.soundprog);
		}
	args.append(sound);
	for (QStringList::Iterator it = args.begin(); it != args.end(); ++it ) {
       		fprintf(stderr, "KK playSound(): %s\n", (*it).latin1());
		}
	QProcess *sndprocess = new QProcess(args, kadu);
	sndprocess->start();
	delete sndprocess;
}

void ChangeUserStatus(uin_t uin, unsigned int new_status) {
	int num = kadu->userbox->numItemsVisible();
	QString tmpstr;

	for (int i = 0; i < num; i++) {
		tmpstr = kadu->userbox->text(i);

		if (pending.pendingMsgs(uin))
			return;

		if (!tmpstr.compare(userlist.byUin(uin).altnick)) {
			QPixmap * gg_st;
			if (new_status == GG_STATUS_AVAIL)
				gg_st = new QPixmap((const char**)gg_act_xpm);
			else
				if (new_status == GG_STATUS_BUSY)
					gg_st = new QPixmap((const char**)gg_busy_xpm);
				else
					if (new_status == GG_STATUS_BUSY_DESCR)
						gg_st = new QPixmap((const char**)gg_busydescr_xpm);
					else
						if (new_status == GG_STATUS_NOT_AVAIL)
							gg_st = new QPixmap((const char**)gg_inact_xpm);
						else
							if (new_status == GG_STATUS_NOT_AVAIL_DESCR)
								gg_st = new QPixmap((const char**)gg_inactdescr_xpm);
							else
								if (new_status == GG_STATUS_AVAIL_DESCR)
									gg_st = new QPixmap((const char**)gg_actdescr_xpm);
								else
									if (new_status == GG_STATUS_INVISIBLE2)
										gg_st = new QPixmap((const char**)gg_invi_xpm);
									else
										if (new_status == GG_STATUS_INVISIBLE_DESCR)
											gg_st = new QPixmap((const char**)gg_invidescr_xpm);
										else
											if (new_status == GG_STATUS_BLOCKED)	
												gg_st = new QPixmap((const char**)gg_stop_xpm);
											else
												gg_st = new QPixmap((const char**)gg_inact_xpm);

			kadu->userbox->changeItem(*gg_st, userlist.byUin(uin).altnick, i);
			delete gg_st;
			}
		}
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
	    		playSound(config.soundnotify);

		}
}

void eventGotUserlist(struct gg_event * e) {
	struct gg_notify_reply *n = e->event.notify;
	unsigned int oldstatus;
	int i;
	
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

		oldstatus = userlist.byUin(n->uin).status;

		if (n->status == GG_STATUS_AVAIL)
			fprintf(stderr, "KK eventGotUserlist(): User %d went online\n", n->uin);
		else
			if (n->status == GG_STATUS_BUSY)
				fprintf(stderr, "KK eventGotUserlist(): User %d went busy\n", n->uin);
			else
				if (n->status == GG_STATUS_NOT_AVAIL && (userlist.byUin(n->uin).status == GG_STATUS_NOT_AVAIL
					|| userlist.byUin(n->uin).status == GG_STATUS_INVISIBLE2)) {
		    			fprintf(stderr, "KK eventGotUserlist(): User %d went offline (probably invisible ;))\n", n->uin);
					UserListElement &ule = userlist.byUin(n->uin);
					ule.time_to_death = 300;
					}
				else
					if (n->status == GG_STATUS_NOT_AVAIL && userlist.byUin(n->uin).status != GG_STATUS_NOT_AVAIL)
						fprintf(stderr, "KK eventGotUserlist(): User %d went offline\n", n->uin);
					else
						if (n->status == GG_STATUS_BLOCKED)
							fprintf(stderr, "KK eventGotUserlist(): User %d has blocked us\n", n->uin);
						else
							fprintf(stderr, "KK eventGotUserlist(): Unknown status for user %d: %d\n", n->uin, n->status);

		if (n->status != GG_STATUS_NOT_AVAIL)
			user.status = n->status;
		else
			if (n->status == GG_STATUS_NOT_AVAIL && (userlist.byUin(n->uin).status == GG_STATUS_NOT_AVAIL
				|| userlist.byUin(n->uin).status == GG_STATUS_INVISIBLE2))
				user.status = GG_STATUS_INVISIBLE2;
			else
				user.status = GG_STATUS_NOT_AVAIL;

		for (i = 0; i < chats.count(); i++)
			if (chats[i].uins.contains(n->uin))
				chats[i].ptr->setTitle();

		ifNotify(n->uin, n->status, oldstatus);

		n++;

		if (user.description)
			user.description.truncate(0);
		
		}

}

void eventGotUserlistWithDescription(struct gg_event *e) {
	struct gg_notify_reply *n = e->event.notify_descr.notify;
	unsigned int oldstatus;
	int i;
	
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

		cp_to_iso((unsigned char *)e->event.notify_descr.descr);
		user.status = n->status;
		user.description.append(__c2q(e->event.notify_descr.descr));
		
		if (n->status == GG_STATUS_BUSY_DESCR)
			fprintf(stderr, "KK eventGotUserlistWithDescription(): User %d went busy with descr.\n", n->uin);
		else
			if (n->status == GG_STATUS_NOT_AVAIL_DESCR)
				fprintf(stderr, "KK eventGotUserlistWithDescription(): User %d went offline with descr.\n", n->uin);
			else
				if (n->status == GG_STATUS_AVAIL_DESCR)
					fprintf(stderr, "KK eventGotUserlistWithDescription(): User %d went online with descr.\n", n->uin);
				else
					if (n->status == GG_STATUS_INVISIBLE_DESCR)
						fprintf(stderr, "KK eventGotUserlistWithDescription(): User %d went invisible with descr.\n", n->uin);

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
		fprintf(stderr, "KK eventGotUserlist(): buddy %d not in list. Damned server!\n", e->event.status.uin);
		gg_remove_notify(sess, e->event.status.uin);
		return;
		}

	oldstatus = user.status;

	user.status = e->event.status.status;

	if (user.description)
		user.description.truncate(0);
	if (ifStatusWithDescription(user.status))
		user.description.append(__c2q(e->event.status.descr));
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
