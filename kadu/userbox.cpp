/***************************************************************************
                          userbox.cpp  -  description
                             -------------------
    begin                : Sat Mar 9 2002
    copyright            : (C) 2002 by tomee
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

#include <klocale.h>

//
#include "kadu.h"
#include "userbox.h"
#include "pixmaps.h"
//

UserBox::UserBox(QWidget* parent,const char* name,WFlags f)
	: QListBox(parent,name),QToolTip(viewport())

{
	UserBoxes.append(this);
	setSelectionMode(QListBox::Extended);
}

UserBox::~UserBox()
{
	UserBoxes.remove(this);
};

void UserBox::maybeTip(const QPoint &c)
{
	QListBoxItem* item = static_cast<QListBoxItem*>(itemAt(c));

	if(item)
	{
		QRect r(itemRect(item));
		QString s;

		switch (userlist.byAltNick(item->text()).status)
		{
			case GG_STATUS_AVAIL:
				s = i18n("<I>Available</I>");
				break;
			case GG_STATUS_BUSY:
				s = i18n("<I>Busy</I>");
				break;
			case GG_STATUS_NOT_AVAIL:
				s = i18n("<nobr><I>Not available</I></nobr>");
				break;
			case GG_STATUS_INVISIBLE2:
				s = i18n("<I>Invisible</I>");
				break;
			case GG_STATUS_BUSY_DESCR:
				s = i18n("<nobr><I>Busy <B>(d.)</B></I></nobr>");
				break;
			case GG_STATUS_NOT_AVAIL_DESCR:
				s = i18n("<nobr><I>Not available <B>(d.)</B></I></nobr>");
				break;
			case GG_STATUS_AVAIL_DESCR:
				s = i18n("<nobr><I>Available <B>(d.)</B></I></nobr>");
				break;
			default:
				s = i18n("<nobr><I>Unknown status</I></nobr>");
				break;
		};
		QString desc=userlist.byAltNick(item->text()).description;
		if (desc!="")
		{
			s += "<BR><BR>";
			s += i18n("<B>Description:</B><BR>");
			s += desc;
		};
		tip(r, s);
	};
}

void UserBox::sortUinsByAltNick(QValueList<uin_t> &uins) {
	int i, j;
	uin_t uin;
	bool stop;

	if (uins.count() < 2)
		return;

	do {
		stop = true;
		for (i = 0; i < uins.count() - 1; i++)
			if (userlist.byUin(uins[i]).altnick > userlist.byUin(uins[i+1]).altnick) {
				uin = uins[i];
				uins[i] = uins[i+1];
				uins[i+1] = uin;
				stop = false;
				}	
	} while (!stop);
}

void UserBox::refresh()
{
	fprintf(stderr, "KK UserBox::refresh()\n");
	
	// Najpierw dzielimy uzytkownikow na trzy grupy
	QValueList<uin_t> a_users;
	QValueList<uin_t> i_users;
	QValueList<uin_t> n_users;
	for (int i = 0; i < Users.count(); i++)
	{
		UserListElement &user = userlist.byAltNick(Users[i]);
		switch (user.status)
		{
			case GG_STATUS_AVAIL:
			case GG_STATUS_AVAIL_DESCR:
			case GG_STATUS_BUSY:
			case GG_STATUS_BUSY_DESCR:
				a_users.append(user.uin);
				break;
			case GG_STATUS_INVISIBLE_DESCR:
			case GG_STATUS_INVISIBLE2:
				i_users.append(user.uin);
				break;
			default:
				n_users.append(user.uin);
		};
	};
	sortUinsByAltNick(a_users);
	sortUinsByAltNick(i_users);
	sortUinsByAltNick(n_users);
	// Czyscimy liste
	clear();
	// Dodajemy aktywnych
	for (int i = 0; i < a_users.count(); i++)
	{
		UserListElement &user = userlist.byUin(a_users[i]);
		if (pending.pendingMsgs(user.uin))
		{
	    		insertItem(QPixmap((const char **)gg_msg_xpm), user.altnick);
		}
		else
		{
			switch (user.status)
			{
				case GG_STATUS_AVAIL:
		    			insertItem(QPixmap((const char **)gg_act_xpm), user.altnick);			
		    			break;
				case GG_STATUS_AVAIL_DESCR:
		    			insertItem(QPixmap((const char **)gg_actdescr_xpm), user.altnick);			
		    			break;
				case GG_STATUS_BUSY:
		    			insertItem(QPixmap((const char **)gg_busy_xpm), user.altnick);			
		    			break;
				case GG_STATUS_BUSY_DESCR:
		    			insertItem(QPixmap((const char **)gg_busydescr_xpm), user.altnick);			
		    			break;
			};
		};
	};	
	// Dodajemy niewidocznych
	for (int i = 0; i < i_users.count(); i++)
	{
		UserListElement &user = userlist.byUin(i_users[i]);
		if (pending.pendingMsgs(user.uin))
		{
	    		insertItem(QPixmap((const char **)gg_msg_xpm), user.altnick);
		}
		else
		{
			switch (user.status)
			{
				case GG_STATUS_INVISIBLE_DESCR:
		    			insertItem(QPixmap((const char **)gg_invidescr_xpm), user.altnick);			
    		    			break;
				case GG_STATUS_INVISIBLE2:
		    			insertItem(QPixmap((const char **)gg_invi_xpm), user.altnick);			
		    			break;
			};
		};
	};	
	// Dodajemy nieaktywnych
	for (int i = 0; i < n_users.count(); i++)
	{
		UserListElement &user = userlist.byUin(n_users[i]);
		if (pending.pendingMsgs(user.uin))
		{
	    		insertItem(QPixmap((const char **)gg_msg_xpm), user.altnick);
		}
		else
		{
			switch (user.status)
			{
				case GG_STATUS_NOT_AVAIL_DESCR:
		    			insertItem(QPixmap((const char **)gg_inactdescr_xpm), user.altnick);			
    		    			break;
				default:
		    			insertItem(QPixmap((const char **)gg_inact_xpm), user.altnick);			
		    			break;
			};
		};
	};	
};

void UserBox::addUser(const QString &altnick)
{
	Users.append(altnick);
};

void UserBox::removeUser(const QString &altnick)
{
	Users.remove(altnick);
};

/*void ChangeUserStatus (unsigned int uin, int new_status) {
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
}*/

void UserBox::changeAllToInactive()
{
	QPixmap qp_inact((const char **)gg_inact_xpm);
	for(int i=0; i<count(); i++)
		changeItem(qp_inact,item(i)->text(),i);
};

/////////////////////////////////////////////////////////

void UserBox::all_refresh()
{
	for(int i=0; i<UserBoxes.size(); i++)
		UserBoxes[i]->refresh();
};

void UserBox::all_removeUser(QString &altnick)
{
	for(int i=0; i<UserBoxes.size(); i++)
		UserBoxes[i]->removeUser(altnick);
};

void UserBox::all_changeAllToInactive()
{
	for(int i=0; i<UserBoxes.size(); i++)
		UserBoxes[i]->changeAllToInactive();	
};

QValueList<UserBox *> UserBox::UserBoxes;

#include "userbox.moc"
