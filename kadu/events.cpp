/***************************************************************************
                          events.cpp  -  description
                             -------------------
    begin                : Thu Sep 6 2001
    copyright            : (C) 2001 by tomee
    email                : tomee@cpi.pl
 ***************************************************************************/

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
#include <qmultilineedit.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlistbox.h>
#include <qpixmap.h>
#include <qcombobox.h>
#include <qstring.h>
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
#include "../libgadu/lib/libgadu.h"

#include "kadu.h"
#include "pixmaps.h"
#include "chat.h"
#include "message.h"
#include "history.h"


char *get_token(char **ptr, char sep)
{
	char *foo, *res;

	if (!ptr || !sep || !*ptr)
		return NULL;

	res = *ptr;

	if (!(foo = strchr(*ptr, sep)))
		*ptr += strlen(*ptr);
	else {
		*ptr = foo + 1;
		*foo = 0;
	}

	return res;
}

char *read_file(FILE *f) {
	char buf[1024], *nju, *res = NULL;

	while (fgets(buf, sizeof(buf) - 1, f)) {
		int new_size = ((res) ? strlen(res) : 0) + strlen(buf) + 1;

		if (!((void *)nju = realloc(res, new_size))) {
			/* jesli brakuje pamieci, pomijamy reszte linii */
			if (strchr(buf, '\n'))
				break;
			else
				continue;
		}
		if (!res)
			*nju = 0;
		res = nju;
		strcpy(res + strlen(res), buf);

		if (strchr(buf, '\n'))
			break;
	}

	if (res && strlen(res) > 0 && res[strlen(res) - 1] == '\n')
		res[strlen(res) - 1] = 0;
	if (res && strlen(res) > 0 && res[strlen(res) - 1] == '\r')
		res[strlen(res) - 1] = 0;


	return res;
}


int readUserlist() {
    char * path;
    struct passwd *pw;

    if (!(pw = getpwuid(getuid())))
	path = getenv("HOME");
    path = pw->pw_dir;

    char * path2 = "/.gg/userlist";
    char buffer[255];
    snprintf(buffer,255,"%s%s",path,path2);

    printf("KK readUserlist(): Opening userlist file: %s\n", buffer);

    FILE *f;
    char * buf;
    int i = 0;
    
    if (!(f = fopen(buffer, "r"))) {
	fprintf(stderr, "KK readUserlist(): Error opening userlist file");
	return -1;
	}

    userlist_count = 0;

    printf("KK readUserlist(): File opened successfuly\n");

    while ((buf = read_file(f))) {
	char *comment;

	if (buf[0] == '#') {
	    free(buf);
	    continue;
	    }

	if (!strchr(buf, ';')) {
	    if (!(comment = strchr(buf, ' '))) {
		free(buf);
		continue;
		}
	    
	    uin_t uin;
	    uin = strtol(buf, NULL, 0);

	    if (!uin) {
		free(buf);
		continue;
		}

	    userlist.resize(userlist.size() + 1);
	    i = userlist.size() - 1;
	    userlist[i].uin = uin;
	    userlist[i].first_name = NULL;
	    userlist[i].last_name = NULL;
	    userlist[i].nickname = NULL;
	    userlist[i].anonymous = false;
	    userlist[i].comment = strdup(++comment);
	    userlist[i].mobile = NULL;
	    userlist[i].ip = 0;
	    userlist[i].port = 0;
	    userlist[i].group = NULL;
	    }
	else {
	    char *first_name, *last_name, *nickname, *comment, *mobile, *group, *uin,
	    *foo = buf;
	    uin_t uint;
	    
	    first_name = get_token(&foo, ';');
	    last_name = get_token(&foo, ';');
/*		nickname = get_token(&foo, ';');
		comment = get_token(&foo, ';'); */
	    comment = get_token(&foo, ';');
	    nickname = get_token(&foo, ';');
	    mobile = get_token(&foo, ';');
	    group = get_token(&foo, ';');
	    uin = get_token(&foo, ';');

      /* load groups */
/*
      if (group != NULL && QString::compare(group, "") != 0) {
        bool already = false;
        for (int f = 0; f < grouplist.size(); f++) {
          if (QString::compare(__c2q(grouplist[f].name), __c2q(group)) == 0) {
            already = true;
            break;
            }
          }

        if (!already) {
          grouplist.resize(grouplist.size()+1);
          if (grouplist.size() > 1)
            grouplist[grouplist.size()-1].number = grouplist[grouplist.size()-2].number + 1;
          else
            grouplist[grouplist.size()-1].number = 601;
          grouplist[grouplist.size()-1].name = strdup(group);
        }

      } */
	    
	    if (!uin || !(uint = strtol(uin, NULL, 0))) {
		free(buf);
		continue;
		}

	    userlist.resize(userlist.size() + 1);
	    userlist[i].uin = uint;
	    userlist[i].first_name = strdup(first_name);
	    cp_to_iso((unsigned char *)userlist[i].first_name);
	    userlist[i].last_name = strdup(last_name);
	    cp_to_iso((unsigned char *)userlist[i].last_name);
	    userlist[i].nickname = strdup(nickname);
	    cp_to_iso((unsigned char *)userlist[i].nickname);
	    userlist[i].comment = strdup(comment);
	    cp_to_iso((unsigned char *)userlist[i].comment);
	    userlist[i].mobile = strdup(mobile);
	    userlist[i].group = strdup(group);
	    cp_to_iso((unsigned char *)userlist[i].group);

	    /* if the nickname isn't defined explicitly, try to guess it */
	    if (!QString::compare(userlist[i].nickname, ""))
		if (!QString::compare(userlist[i].comment, ""))
		    strcpy(userlist[i].nickname, userlist[i].first_name);
		else
		    strcpy(userlist[i].nickname, userlist[i].comment);
	    }

	free(buf);

	userlist[i].status = GG_STATUS_NOT_AVAIL;
	userlist[i].description = NULL;
	
	i++;
	}
    userlist_count = i;

    fclose(f);

    return 0;
}


void sigchldHndl (int whatever) {

 while ((wait3(NULL, WNOHANG, NULL)) > 0);
}


void eventRecvMsg(int msgclass, uin_t sender, unsigned char * msg, time_t time,int formats_count=0,struct gg_msg_format * formats=NULL) {
    QString tmp;
    
	if (isIgnored(sender)) return;

	/* check whether it's a system message */
	if (sender == 0) {
		if (msgclass <= config.sysmsgidx) {
		        std::cout << "KK Already had this message, ignoring" << std::endl;
			return;
			}
     config.sysmsgidx = msgclass;
			printf("KK System message index %d\n", msgclass);
			sender = config.uin;
		}

	QString nick;
	nick = UinToUser(sender);
	cp_to_iso(msg);

	if (config.logmessages && sender != config.uin)
		appendHistory(sender,msg,FALSE,time);

 
	int i; bool yup = FALSE;
	i = 0;
	while (i < chats.size() && chats[i].uin != (unsigned int)sender)
	    i++;

	if (msgclass == GG_CLASS_CHAT && i < chats.size()) {
	    tmp = __c2q((const char *)msg);
	    chats[i].ptr->checkPresence(sender, &tmp, time);
  	    chats[i].ptr->playChatSound();
	    
	    return;
	    }

	playSound(config.soundmsg);

	for (i = 0; i < pending.size(); i++)
		if (pending[i].uin == 0)
			break;

	if (i == pending.size()) {
		pending.resize(pending.size() + 1);
		fprintf(stderr, "KK eventRecvMsg(): New buffer size: %d\n",pending.size());
		i = pending.size()-1;
		}

	pending[i].uin = sender;
	pending[i].msgclass = msgclass;
	pending[i].msg = new QString;
	pending[i].msg->append(__c2q((const char *)msg));
	pending[i].time = time;
	
	fprintf(stderr, "KK eventRecvMsg(): Message allocated to slot %d\n", i);
	fprintf(stderr, "KK eventRecvMsg(): Got message from %d (%s) saying \"%s\"\n", sender, (const char *)nick.local8Bit(), msg);

	kadu->syncUserlist();
	kadu->sortUsers();
	dw->setType((char **)gg_msg_xpm);


    if (config.raise) {
	kadu->showNormal();
	kadu->setFocus();
        }

    if (sender == config.uin) {
	rMessage *rmsg;
	rmsg = new rMessage("System", i);
	rmsg->show();
	}
}

void playSound(char * sound) {
if (!config.playsound) return;

	if (!mute) {
	if ((QString::compare(sound,NULL) == 0) || (QString::compare(sound,"") == 0)) {
		fprintf(stderr,"KK No sound file specified?\n");
		return;
		}

	char path[511];
	pid_t nasz_pid;
	nasz_pid = fork();
if (nasz_pid == 0) {

if (config.playartsdsp) {
	if (config.soundvolctrl)
		snprintf(path, sizeof(path), "artsdsp %s -v %.2f %s", config.soundprog, config.soundvol, sound);
	else
		snprintf(path, sizeof(path), "artsdsp %s %s", config.soundprog, sound);
} else {
	if (config.soundvolctrl)
		snprintf(path, sizeof(path), "%s -v %.2f %s", config.soundprog, config.soundvol, sound);
	else	
		snprintf(path, sizeof(path), "%s %s", config.soundprog, sound);
	}

	system(path);
	exit(0);
} else {
struct sigaction sigact;
sigact.sa_handler = sigchldHndl;
sigact.sa_flags = 0;
sigact.sa_restorer = NULL;
if (sigaction(SIGCHLD,&sigact,NULL) < 0)
      perror("sigaction");
}
	}

}

void ChangeUserStatus (unsigned int uin, int new_status) {
    int num = mylist->numItemsVisible();
    QString tmpstr;

    for (int i = 0; i < num; i++) {
	tmpstr = mylist->text(i);
	
	for (int j = 0; j < pending.size(); j++)
	    if (pending[j].uin == uin)
		return;

	if (!tmpstr.compare(__c2q(UinToUser(uin)))) {
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
					    gg_st = new QPixmap((const char**)gg_inact_xpm);

	    mylist->changeItem(*gg_st, __c2q(UinToUser(uin)), i);
	    delete gg_st;
	    }
	}
}

void ifNotify(uin_t uin, unsigned long status, unsigned long oldstatus)
{

    if (config.notifyglobal && config.notifies.contains(QString::number(uin)) && (status == GG_STATUS_AVAIL ||
	status == GG_STATUS_AVAIL_DESCR || status == GG_STATUS_BUSY || status == GG_STATUS_BUSY_DESCR) &&
	(oldstatus == GG_STATUS_NOT_AVAIL || oldstatus == GG_STATUS_NOT_AVAIL_DESCR || oldstatus == GG_STATUS_INVISIBLE ||
	oldstatus == GG_STATUS_INVISIBLE_DESCR || oldstatus == GG_STATUS_INVISIBLE2))
	{
	fprintf(stderr, "KK Notify about user\n");
    
	if (config.notifydialog) {		
	    char mes[512];
	    snprintf(mes, sizeof(mes), i18n("User %s is available"), (const char *)UinToUser(uin).local8Bit());
	    QMessageBox::information(0, i18n("User notify"), mes);		
	    }

	if (config.notifysound)
	    playSound(config.soundnotify);

	}
}


void eventGotUserlist(struct gg_event * e) {
    struct gg_notify_reply *n = e->event.notify;
    unsigned long oldstatus;
    int i;

    while (n->uin) {
	i = 0;
	while (i < userlist.size() && userlist[i].uin != n->uin)
	    i++;

	if (i >= userlist.size()) {
	    fprintf(stderr, "KK eventGotUserlist(): buddy %d not in list. Damned server!\n", n->uin);
  	    gg_remove_notify(&sess, n->uin);
    	    n++;
    	    continue;
	    }


	userlist[i].ip = n->remote_ip;
	userlist[i].port = n->remote_port;

	oldstatus = GetStatusFromUserlist(n->uin);
    
	if (n->status == GG_STATUS_AVAIL)
	    fprintf(stderr, "KK eventGotUserlist(): User %d went online\n", n->uin);
	else
	    if (n->status == GG_STATUS_BUSY)
		fprintf(stderr, "KK eventGotUserlist(): User %d went busy\n", n->uin);
	    else
		if (n->status == GG_STATUS_NOT_AVAIL && (GetStatusFromUserlist(n->uin) == GG_STATUS_NOT_AVAIL
		    || GetStatusFromUserlist(n->uin) == GG_STATUS_INVISIBLE2))
		    fprintf(stderr, "KK eventGotUserlist(): User %d went offline (probably invisible ;))\n", n->uin);
		else
		    if (n->status == GG_STATUS_NOT_AVAIL && GetStatusFromUserlist(n->uin) != GG_STATUS_NOT_AVAIL)
			fprintf(stderr, "KK eventGotUserlist(): User %d went offline\n", n->uin);
		    else
			fprintf(stderr, "KK eventGotUserlist(): Unknown status for user %d: %d\n", n->uin, n->status);

	if (n->status != GG_STATUS_NOT_AVAIL)
	    UinToUserlistEntry(n->uin, n->status);
	else
	    if (n->status == GG_STATUS_NOT_AVAIL && (GetStatusFromUserlist(n->uin) == GG_STATUS_NOT_AVAIL
	        || GetStatusFromUserlist(n->uin) == GG_STATUS_INVISIBLE2))
		UinToUserlistEntry(n->uin, GG_STATUS_INVISIBLE2);
	    else
		UinToUserlistEntry(n->uin, GG_STATUS_NOT_AVAIL);

	ifNotify(n->uin, n->status, oldstatus);
	n++;

	if (userlist[i].description) {
//	    delete userlist[i].description;
	    userlist[i].description->truncate(0);
//	    userlist[i].description = NULL;
	    }
	}

    kadu->syncUserlist();
    
}

void eventGotUserlistWithDescription(struct gg_event * e) {
    struct gg_notify_reply *n = e->event.notify_descr.notify;
    unsigned long oldstatus;
    int i;
      
    while (n->uin) {
  
	i = 0;
	while (i < userlist.size() && userlist[i].uin != n->uin)
	    i++;
		
	if (i >= userlist.size())
	    {
    	    fprintf(stderr, "KK eventGotUserlist(): buddy %d not in list. Damned server!\n", n->uin);
  	    gg_remove_notify(&sess, n->uin);
    	    n++;
	    continue;
	    }

	userlist[i].ip = n->remote_ip;
	userlist[i].port = n->remote_port;

	oldstatus = userlist[i].status;
		
	if (userlist[i].description)
	    userlist[i].description->truncate(0);
	else
	    userlist[i].description = new QString;

	if (n->status == GG_STATUS_BUSY_DESCR) {
	    fprintf(stderr, "KK eventGotUserlistWithDescription(): User %d went busy with descr.\n", n->uin);
	    cp_to_iso((unsigned char *)e->event.notify_descr.descr);
	    userlist[i].description->append(__c2q(e->event.notify_descr.descr));
	    UinToUserlistEntry(n->uin, GG_STATUS_BUSY_DESCR);
//		ChangeUserStatus(n->uin, GG_STATUS_BUSY_DESCR);
	    }
	else
	    if (n->status == GG_STATUS_NOT_AVAIL_DESCR) {
		fprintf(stderr, "KK eventGotUserlistWithDescription(): User %d went offline with descr.\n", n->uin);
		cp_to_iso((unsigned char *)e->event.notify_descr.descr);
		userlist[i].description->append(__c2q(e->event.notify_descr.descr));
//		ChangeUserStatus(n->uin, GG_STATUS_NOT_AVAIL_DESCR);
		UinToUserlistEntry(n->uin, GG_STATUS_NOT_AVAIL_DESCR);
		}
	     else
		if (n->status == GG_STATUS_AVAIL_DESCR) {
		    fprintf(stderr, "KK eventGotUserlistWithDescription(): User %d went online with descr.\n", n->uin);
		    cp_to_iso((unsigned char *)e->event.notify_descr.descr);
		    userlist[i].description->append(__c2q(e->event.notify_descr.descr));
//			ChangeUserStatus(n->uin, GG_STATUS_AVAIL_DESCR);
		    UinToUserlistEntry(n->uin, GG_STATUS_AVAIL_DESCR);
		    }
		else
		    if (n->status == GG_STATUS_INVISIBLE_DESCR) {
			fprintf(stderr, "KK eventGotUserlistWithDescription(): User %d went invisible with descr.\n", n->uin);
			cp_to_iso((unsigned char *)e->event.notify_descr.descr);
			userlist[i].description->append(__c2q(e->event.notify_descr.descr));
//			ChangeUserStatus(n->uin, GG_STATUS_INVISIBLE_DESCR);
			UinToUserlistEntry(n->uin, GG_STATUS_INVISIBLE_DESCR);
			}

	ifNotify(n->uin, n->status, oldstatus);
	n++;
	}
	
    kadu->syncUserlist();
}


void eventStatusChange(struct gg_event * e) {
    fprintf(stderr, "KK eventStatusChange(): User %d went %d\n", e->event.status.uin,  e->event.status.status);

    int i;
    unsigned long oldstatus;
    
    i = 0;	
    while (i < userlist.size() && userlist[i].uin != e->event.status.uin)
	i++;
    if (i >= userlist.size()) {
	// ignore!
	fprintf(stderr, "KK eventGotUserlist(): buddy %d not in list. Damned server!\n", e->event.status.uin);
  	gg_remove_notify(&sess, e->event.status.uin);
	return;
	}

    if (e->event.status.status == GG_STATUS_AVAIL) {
//		ChangeUserStatus(e->event.status.uin, GG_STATUS_AVAIL);
	UinToUserlistEntry(e->event.status.uin, GG_STATUS_AVAIL);

	i = 0;
	while (i < userlist.size() && userlist[i].uin != e->event.status.uin)
	    i++;

	oldstatus = userlist[i].status;
		
	if (userlist[i].description) {
    	    userlist[i].description->truncate(0);
//		delete userlist[i].description;
	    userlist[i].description = NULL;
	    }
	}
    else
	if (e->event.status.status == GG_STATUS_AVAIL_DESCR) {
//		ChangeUserStatus(e->event.status.uin, GG_STATUS_AVAIL_DESCR);
	    UinToUserlistEntry(e->event.status.uin, GG_STATUS_AVAIL_DESCR);

	    i = 0;
	    while (i < userlist.size() && userlist[i].uin != e->event.status.uin)
		i++;

	    if (userlist[i].description)
		userlist[i].description->truncate(0);
	    else
		userlist[i].description = new QString;

	    userlist[i].description->append(__c2q(e->event.status.descr));
	    }
	else
	    if (e->event.status.status == GG_STATUS_BUSY) {
//		ChangeUserStatus(e->event.status.uin, GG_STATUS_BUSY);
		UinToUserlistEntry(e->event.status.uin, GG_STATUS_BUSY);

		i = 0;
		while (i < userlist.size() && userlist[i].uin != e->event.status.uin)
		    i++;

		if (userlist[i].description) {
		    userlist[i].description->truncate(0);
//			delete userlist[i].description;
		    userlist[i].description = NULL;
		    }
		}
	    else
		if (e->event.status.status == GG_STATUS_BUSY_DESCR) {
//			ChangeUserStatus(e->event.status.uin, GG_STATUS_BUSY_DESCR);
		    UinToUserlistEntry(e->event.status.uin, GG_STATUS_BUSY_DESCR);

		    i = 0;
		    while (i < userlist.size() && userlist[i].uin != e->event.status.uin)
			i++;

		    if (userlist[i].description)
			userlist[i].description->truncate(0);
		    else
			userlist[i].description = new QString;

		    userlist[i].description->append(__c2q(e->event.status.descr));
		    }
		else
		    if (e->event.status.status == GG_STATUS_NOT_AVAIL) {
//			ChangeUserStatus(e->event.status.uin, GG_STATUS_NOT_AVAIL);
			UinToUserlistEntry(e->event.status.uin, GG_STATUS_NOT_AVAIL);

			i = 0;
			while (i < userlist.size() && userlist[i].uin != e->event.status.uin)
			    i++;
			
			userlist[i].ip = 0;
			userlist[i].port = 0;

			if (userlist[i].description) {
    			    userlist[i].description->truncate(0);
//				delete userlist[i].description;
			    userlist[i].description = NULL;
			    }
			}
		    else
			if (e->event.status.status == GG_STATUS_INVISIBLE_DESCR) {
//				ChangeUserStatus(e->event.status.uin, GG_STATUS_INVISIBLE_DESCR);
			    UinToUserlistEntry(e->event.status.uin, GG_STATUS_INVISIBLE_DESCR);

			    i = 0;
			    while (i < userlist.size() && userlist[i].uin != e->event.status.uin)
			        i++;

			    if (userlist[i].description)
			        userlist[i].description->truncate(0);
			    else
			        userlist[i].description = new QString;

			    userlist[i].description->append(__c2q(e->event.status.descr));
			    }
			else
			    if (e->event.status.status == GG_STATUS_NOT_AVAIL_DESCR) {
//				ChangeUserStatus(e->event.status.uin, GG_STATUS_NOT_AVAIL_DESCR);
				UinToUserlistEntry(e->event.status.uin, GG_STATUS_NOT_AVAIL_DESCR);

				i = 0;
				while (i < userlist.size() && userlist[i].uin != e->event.status.uin)
				    i++;

				if (userlist[i].description)
				    userlist[i].description->truncate(0);
				else
				    userlist[i].description = new QString;

				fprintf(stderr, "KK eventStatusChange(): finished\n");

				userlist[i].description->append(__c2q(e->event.status.descr));

				userlist[i].ip = 0;
				userlist[i].port = 0;
				}

    kadu->syncUserlist();
    ifNotify(e->event.status.uin, e->event.status.status, oldstatus);
}

void kadu_debug(int debuglevel, char * message) {
//		fprintf(stderr, "Debug type %i: %s\n", debuglevel, message);
	;
}

void ackHandler(int seq) {
    int i,j,k;
    for (i = 0; i < acks.size(); i++)
	if (acks[i].seq == seq && acks[i].ptr)
	    if (acks[i].type < 2)
		((Message *)acks[i].ptr)->gotAck();
	    else {
		j = 0;
		while (j < chats.size() && chats[j].ptr != acks[i].ptr)
		    j++;
		if (j < chats.size())
		    ((Chat *)acks[i].ptr)->writeMyMessage();
		for (k = i + 1; k < acks.size(); k++) {
		    acks[i-1].seq = acks[i].seq;
		    acks[i-1].type = acks[i].type;
		    acks[i-1].ptr = acks[i].ptr;
		    }
		acks.resize(acks.size() - 1);
		}
}
