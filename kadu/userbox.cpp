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
/*	QListBoxItem* item = static_cast<QListBoxItem*>(itemAt(c));

	if(item)
	{
		QRect r(itemRect(item));
		QString s;
		int i=FindCommentInUserlist(item->text());

		switch (userlist[i].status)
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
		if (descriptionForUser(item) != -1)
		{
			s += "<BR><BR>";
			s += i18n("<B>Description:</B><BR>");
			s += QString(*userlist[descriptionForUser(item)].description);
		};
		tip(r, s);
	};*/
}

int UserBox::descriptionForUser(QListBoxItem * s)
{
/*	int i = FindCommentInUserlist(s->text());
	if(userlist[i].description==0)*/
		return -1;
/*	if(userlist[i].description->length()<=0)
		return -1;
	return i;*/
};

int UserBox::findUinInUserlist(uin_t uin)
{
	int i;
	for(i=0; i<userlist.size(); i++)
		if(userlist[i].uin==uin)
			return i;
	fprintf(stderr, "KK UserBox::findUinInUserList(): userlist[] out bound!\n");
	return -1;
};

int UserBox::findCommentInUserlist(QString& comment)
{
	int i;
	for(i=0; i<userlist.size(); i++)
		if(userlist[i].comment==comment)
			return i;
	fprintf(stderr, "KK UserBox::findCommentInUserList(): userlist[] out bound!\n");
	return -1;
};

/* don't worry if you can't follow this. I can't either. */
/* should brew beer, sorts users instead */
/*void Userbox::sortUsers()
{
    unsigned int i, j, k = 0;
    bool item_selected = 0;
    
    for (i = 0; i < mylist->count(); i++) {
	if (mylist->isSelected(i)) {
	    item_selected = i;
	    break;
	    }
	} // int i

    for (i = 0; i < mylist->count(); i++) {
	j = 0;
	while (j < userlist.size() && QString::compare(__c2q(userlist[j].nickname), mylist->item(i)->text()))
	    j++;
    
	if (userlist[j].status == GG_STATUS_NOT_AVAIL_DESCR) {
	    QPixmap * gg_st;
	    if (ifPendingMessages(userlist[j].uin))
		gg_st = new QPixmap((const char**)gg_msg_xpm);
	    else
		gg_st = new QPixmap((const char**)gg_inactdescr_xpm);		
	    QString tmpstr;
	    tmpstr = mylist->item(i)->text();
	    mylist->changeItem(*gg_st, tmpstr, i);
	    delete gg_st;				
	    }
	else
	    if (userlist[j].status == GG_STATUS_AVAIL || userlist[j].status == GG_STATUS_AVAIL_DESCR) {
		QPixmap * gg_st;

		if (ifPendingMessages(userlist[j].uin))
		    gg_st = new QPixmap((const char**)gg_msg_xpm);
		else
		    if (userlist[j].status == GG_STATUS_AVAIL_DESCR)
			gg_st = new QPixmap((const char**)gg_actdescr_xpm);
    		    else
			gg_st = new QPixmap((const char**)gg_act_xpm);

		QPixmap * gg_st2;
		QString tmpstr, tmpstr2;
		tmpstr = mylist->item(i)->text();
		tmpstr2 = mylist->item(k)->text();
		fprintf(stderr, "KK Kadu::sortUsers(): k: %d, i: %d, j: %d\n", k, i, j);
		gg_st2 = new QPixmap(*mylist->item(k)->pixmap());
		mylist->changeItem(*gg_st, tmpstr, k);
		mylist->changeItem(*gg_st2, tmpstr2, i);
		delete gg_st;
		delete gg_st2;
		struct userlist tmpustr;
		tmpustr = userlist[k];
		userlist[k] = userlist[i];
		userlist[i] = tmpustr;
		k++;
		}
	    else
		if (userlist[j].status == GG_STATUS_BUSY || userlist[j].status == GG_STATUS_BUSY_DESCR) {
		    QPixmap * gg_st;
		    if (ifPendingMessages(userlist[j].uin))
			gg_st = new QPixmap((const char**)gg_msg_xpm);
		    else
			if (userlist[j].status == GG_STATUS_BUSY_DESCR)
			    gg_st = new QPixmap((const char**)gg_busydescr_xpm);
			else
			    gg_st = new QPixmap((const char**)gg_busy_xpm);

		    QPixmap * gg_st2;
		    QString tmpstr, tmpstr2;
		    tmpstr = mylist->item(i)->text();
		    tmpstr2 = mylist->item(k)->text();
		    fprintf(stderr, "KK Kadu::sortUsers(): k: %d, i: %d, j: %d\n", k, i, j);
		    gg_st2 = new QPixmap(*mylist->item(k)->pixmap());
		    mylist->changeItem(*gg_st, tmpstr, k);
		    mylist->changeItem(*gg_st2, tmpstr2, i);
		    delete gg_st;
		    delete gg_st2;
		    struct userlist tmpustr;
		    tmpustr = userlist[k];
		    userlist[k] = userlist[i];
		    userlist[i] = tmpustr;
		    k++;
		    }
		else
		    if (userlist[j].status == GG_STATUS_INVISIBLE2) {
			QPixmap * gg_st;
			if (ifPendingMessages(userlist[j].uin))
			    gg_st = new QPixmap((const char**)gg_msg_xpm);
			else
			    gg_st = new QPixmap((const char**)gg_invi_xpm);
			QPixmap * gg_st2;
			QString tmpstr, tmpstr2;
			tmpstr = mylist->item(i)->text();
			tmpstr2 = mylist->item(k)->text();
			fprintf(stderr, "KK Kadu::sortUsers(): k: %d, i: %d, j: %d\n", k, i, j);
			gg_st2 = new QPixmap(*mylist->item(k)->pixmap());
			mylist->changeItem(*gg_st, tmpstr, k);
			mylist->changeItem(*gg_st2, tmpstr2, i);
			delete gg_st;
			delete gg_st2;
			struct userlist tmpustr;
			tmpustr = userlist[k];
			userlist[k] = userlist[i];
			userlist[i] = tmpustr;
			k++;
			}

	mylist->setSelected(i, false);
	} //int i

    mylist->setSelected(item_selected, true);
}
*/
void UserBox::refresh()
{
/*	clear();	
	for(int i=0; i<Uins.count(); i++)
	{
		int u=FindUinInUserlist(Uins[i]);
		if (ifPendingMessages(Uins[i]))
		{
	    		insertItem(QPixmap((const char **)gg_msg_xpm), __c2q(userlist[u].nickname));
		}
		else
		{
			switch (userlist[u].status)
			{
				case GG_STATUS_AVAIL:
		    			insertItem(QPixmap((const char **)gg_act_xpm), __c2q(userlist[u].nickname));			
		    			break;
				case GG_STATUS_AVAIL_DESCR:
		    			insertItem(QPixmap((const char **)gg_actdescr_xpm), __c2q(userlist[u].nickname));			
		    			break;
				case GG_STATUS_NOT_AVAIL:
		    			insertItem(QPixmap((const char **)gg_inact_xpm), __c2q(userlist[u].nickname));			
		    			break;
				case GG_STATUS_BUSY:
		    			insertItem(QPixmap((const char **)gg_busy_xpm), __c2q(userlist[u].nickname));			
		    			break;
				case GG_STATUS_BUSY_DESCR:
		    			insertItem(QPixmap((const char **)gg_busydescr_xpm), __c2q(userlist[u].nickname));			
		    			break;
				case GG_STATUS_NOT_AVAIL_DESCR:
		    			insertItem(QPixmap((const char **)gg_inactdescr_xpm), __c2q(userlist[u].nickname));			
		    			break;
				case GG_STATUS_INVISIBLE_DESCR:
		    			insertItem(QPixmap((const char **)gg_invidescr_xpm), __c2q(userlist[u].nickname));			
    		    			break;
				case GG_STATUS_INVISIBLE2:
		    			insertItem(QPixmap((const char **)gg_invi_xpm), __c2q(userlist[u].nickname));			
		    			break;
				default:
		    			insertItem(QPixmap((const char **)gg_inact_xpm), __c2q(userlist[u].nickname));			
			};
		};
	};	*/
};

void UserBox::addUin(uin_t uin)
{
	Uins.append(uin);
};

void UserBox::removeUin(uin_t uin)
{
	Uins.remove(uin);
	refresh();	
};

void UserBox::removeUser(QString& username)
{
/*	removeUin(userlist[FindCommentInUserlist(username)].uin);*/
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
/*	QPixmap qp_inact((const char **)gg_inact_xpm);
	for(int i=0; i<count(); i++)
		changeItem(qp_inact,item(i).text,i);*/
};

/////////////////////////////////////////////////////////

void UserBox::all_refresh()
{
	for(int i=0; i<UserBoxes.size(); i++)
		UserBoxes[i]->refresh();
};

void UserBox::all_removeUser(QString& username)
{
	for(int i=0; i<UserBoxes.size(); i++)
		UserBoxes[i]->removeUser(username);
};

void UserBox::all_changeAllToInactive()
{
	for(int i=0; i<UserBoxes.size(); i++)
		UserBoxes[i]->changeAllToInactive();	
};

// The code below will be removed soon

MyListBox::MyListBox ( QWidget * parent, const char * name, WFlags f ) : QListBox(parent, name),
    QToolTip(viewport()) {
    setSelectionMode(QListBox::Extended);
}

void MyListBox::maybeTip(const QPoint &c) {
    QListBoxItem* item = static_cast<QListBoxItem*>(itemAt(c));

    if (item) {
        QRect r(itemRect(item));
        QString s,t;

        int i;
        i = 0;
        while (i < userlist.size() && userlist[i].uin != UserToUin(&(t = item->text())))
            i++;

        switch (userlist[i].status) {
            case GG_STATUS_AVAIL:
                s += i18n("<I>Available</I>");
                break;
            case GG_STATUS_BUSY:
                s += i18n("<I>Busy</I>");
                break;
            case GG_STATUS_NOT_AVAIL:
                s += i18n("<nobr><I>Not available</I></nobr>");
                break;
            case GG_STATUS_INVISIBLE2:
                s += i18n("<I>Invisible</I>");
                break;
            case GG_STATUS_BUSY_DESCR:
                s += i18n("<nobr><I>Busy <B>(d.)</B></I></nobr>");
                break;
            case GG_STATUS_NOT_AVAIL_DESCR:
                s += i18n("<nobr><I>Not available <B>(d.)</B></I></nobr>");
                break;
            case GG_STATUS_AVAIL_DESCR:
                s += i18n("<nobr><I>Available <B>(d.)</B></I></nobr>");
                break;
            default:
                s += i18n("<nobr><I>Unknown status</I></nobr>");
                break;
            }

        if (descriptionForUser(item) != -1) {
            s += "<BR><BR>";
            s += i18n("<B>Description:</B><BR>");
            s += QString(*userlist[descriptionForUser(item)].description);
            }
        
        tip(r, s);
        }
}

int MyListBox::descriptionForUser(QListBoxItem * s) {
    int i;
    QString t;

    i = 0;      
    while (i < userlist.size() && userlist[i].uin != UserToUin(&(t = s->text())))
        i++;

    if (userlist[i].description)
        if (userlist[i].description->length() > 0)
            return i;
        else
            return -1;
    else
        return -1;
}

QValueList<UserBox*> UserBox::UserBoxes;

#include "userbox.moc"
