/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qmap.h>
#include <qregexp.h>
#include <qdragobject.h>
#include <qpainter.h>
#include <qpen.h>

//
#include "kadu.h"
//
#include "userbox.h"
#include "ignore.h"
#include "pixmaps.h"
#include "debug.h"
#include "pending_msgs.h"

KaduListBoxPixmap::KaduListBoxPixmap(const QPixmap &pix, const QString &text)
	: QListBoxPixmap(pix, text)
{
}

void KaduListBoxPixmap::paint(QPainter *painter) {
	UserListElement &user = userlist.byAltNick(text());
	if (user.uin) {
		UinsList uins;
		uins.append(user.uin);
		if (user.blocking) {
			QPen &pen = (QPen &)painter->pen();
			pen.setColor(QColor(255, 0, 0));
			painter->setPen(pen);
			}
		else
			if (isIgnored(uins)) {
				QPen &pen = (QPen &)painter->pen();
				pen.setColor(QColor(192, 192, 0));
				painter->setPen(pen);
				}
			else
				if (user.offline_to_user) {
					QPen &pen = (QPen &)painter->pen();
					pen.setColor(QColor(128, 128, 128));
					painter->setPen(pen);
					}
		}
	QListBoxPixmap::paint(painter);
}

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
				if (!userlist.byAltNick(item->text()).uin)
				    s = i18n("<I>Mobile:</I><B> ")+userlist.byAltNick(item->text()).mobile+"</B>";
				else
				    s = i18n("<nobr><I>Not available</I></nobr>");
				break;
			case GG_STATUS_INVISIBLE:
			case GG_STATUS_INVISIBLE2:
				s = i18n("<I>Invisible</I>");
				break;
			case GG_STATUS_INVISIBLE_DESCR:
				s = i18n("<I>Invisible <B>(d.)</B></I>");
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
			case GG_STATUS_BLOCKED:
				s = i18n("<nobr><I>Blocking</I></nobr>");				
				break;
			default:
				s = i18n("<nobr><I>Unknown status</I></nobr>");
				break;
		};
		QString desc = userlist.byAltNick(item->text()).description;
		if (desc != "")
		{
			s += "<BR><BR>";
			s += i18n("<B>Description:</B><BR>");
			desc.replace(QRegExp("<"), "&lt;");
			desc.replace(QRegExp(">"), "&gt;");
			s += desc;
		};
		tip(r, s);
	};
}

void UserBox::mousePressEvent(QMouseEvent *e) {	
	if (e->button() != RightButton)
		QListBox::mousePressEvent(e);
	else {
		QListBoxItem *item;
		item = itemAt(e->pos());
		if (item) {
			if (!item->isSelected())
				if (!(e->state() & Qt::ControlButton))
					for (int i = 0; i < count(); i++)
						setSelected(i, FALSE);
			setSelected(item, TRUE);
			setCurrentItem(item);
			emit rightButtonClicked(item, e->globalPos());
			}
		}
}

void UserBox::mouseMoveEvent(QMouseEvent* e)
{
    if ((e->state() & LeftButton)&&itemAt(e->pos()))
    {
        QDragObject* d = new QTextDrag(itemAt(e->pos())->text(),this);
	d->dragCopy();
    }
    else
	QListBox::mouseMoveEvent(e);
};

void UserBox::sortUsersByAltNick(QStringList &users) {
	int i, count;
	QString tmp;
	bool stop;

	QMap<QString,QString> usermap;
	for (i = 0; i < users.count(); i++) {
		usermap.insert(users[i].upper(), users[i]);
		users[i] = users[i].upper();
		}
	// sortowanie uzytkownikow
	count = users.count();
	do {
		stop = true;
		for (i = 0; i < count - 1; i++)
			if (users[i].localeAwareCompare(users[i+1]) > 0) {
				tmp = users[i];
				users[i] = users[i+1];
				users[i+1] = tmp;
				stop = false;
				}
		} while (!stop);
//	users.sort();

	for (i = 0; i < users.count(); i++)
		users[i] = usermap[users[i]];
}

void UserBox::refresh()
{
	int i;
	KaduListBoxPixmap *lbp;

	kdebug("UserBox::refresh()\n");

	// Zapamietujemy zaznaczonych uzytkownikow
	QStringList s_users;
	for (i = 0; i < count(); i++)
		if (isSelected(i))
			s_users.append(item(i)->text());
	QString s_user = currentText();
	// Najpierw dzielimy uzytkownikow na cztery grupy
	QStringList a_users;
	QStringList i_users;
	QStringList n_users;
	QStringList b_users;
	for (i = 0; i < Users.count(); i++) {
		UserListElement &user = userlist.byAltNick(Users[i]);
		if (user.uin)
			switch (user.status) {
				case GG_STATUS_AVAIL:
				case GG_STATUS_AVAIL_DESCR:
				case GG_STATUS_BUSY:
				case GG_STATUS_BUSY_DESCR:
				case GG_STATUS_BLOCKED:
					a_users.append(user.altnick);
					break;
				case GG_STATUS_INVISIBLE_DESCR:
				case GG_STATUS_INVISIBLE2:
					i_users.append(user.altnick);
					break;
				default:
					n_users.append(user.altnick);
				}
		else
			b_users.append(user.altnick);
		}
	sortUsersByAltNick(a_users);
	sortUsersByAltNick(i_users);
	sortUsersByAltNick(n_users);
	sortUsersByAltNick(b_users);
	// Czyscimy liste
	clear();
	// Dodajemy aktywnych
	for (i = 0; i < a_users.count(); i++)
	{
		UserListElement &user = userlist.byAltNick(a_users[i]);
		bool has_mobile = user.mobile.length();
		if (pending.pendingMsgs(user.uin)) {
			lbp = new KaduListBoxPixmap(*icons->loadIcon("message"), user.altnick);
			insertItem(lbp);
//			insertItem(*icons->loadIcon("message"), user.altnick);
			}
		else
		{
			QPixmap *pix = NULL;
			switch (user.status) {
				case GG_STATUS_AVAIL:
					if (has_mobile)
						pix = icons->loadIcon("online_m");
					else
		    				pix = icons->loadIcon("online");
		    			break;
				case GG_STATUS_AVAIL_DESCR:
					if (has_mobile)
						pix = icons->loadIcon("online_d_m");
					else
		    				pix = icons->loadIcon("online_d");
		    			break;
				case GG_STATUS_BUSY:
					if (has_mobile)
						pix = icons->loadIcon("busy_m");
					else
						pix = icons->loadIcon("busy");
		    			break;
				case GG_STATUS_BUSY_DESCR:
					if (has_mobile)
						pix = icons->loadIcon("busy_d_m");
					else
						pix = icons->loadIcon("busy_d");
		    			break;
				case GG_STATUS_BLOCKED:
					pix = icons->loadIcon("blocking");
					break;
				};
			if (pix != NULL)
				lbp = new KaduListBoxPixmap(*pix, user.altnick);
//				insertItem(*pix, user.altnick);
			else
				lbp = new KaduListBoxPixmap(*icons->loadIcon("online"), user.altnick);
//				insertItem(*icons->loadIcon("online"), user.altnick);
			insertItem(lbp);
		};
	};	
	// Dodajemy niewidocznych
	for (i = 0; i < i_users.count(); i++) {
		UserListElement &user = userlist.byAltNick(i_users[i]);
		bool has_mobile = user.mobile.length();
		if (pending.pendingMsgs(user.uin)) {
			lbp = new KaduListBoxPixmap(*icons->loadIcon("message"), user.altnick);
			insertItem(lbp);
//	    		insertItem(*icons->loadIcon("message"), user.altnick);
			}
		else {
			QPixmap *pix = NULL;
			switch (user.status) {
				case GG_STATUS_INVISIBLE_DESCR:
					if (has_mobile)
						pix = icons->loadIcon("invisible_d_m");
					else
						pix = icons->loadIcon("invisible_d");
    		    			break;
				case GG_STATUS_INVISIBLE2:
					if (has_mobile)
						pix = icons->loadIcon("invisible_m");
					else
						pix = icons->loadIcon("invisible");
		    			break;
				}
			lbp = new KaduListBoxPixmap(*pix, user.altnick);
			insertItem(lbp);
//			insertItem(*pix, user.altnick);			
			}
		}
	// Dodajemy nieaktywnych
	for (i = 0; i < n_users.count(); i++)
	{
		UserListElement &user = userlist.byAltNick(n_users[i]);
		bool has_mobile = user.mobile.length();
		if (pending.pendingMsgs(user.uin)) {		
			lbp = new KaduListBoxPixmap(*icons->loadIcon("message"), user.altnick);
			insertItem(lbp);
//	    		insertItem(*icons->loadIcon("message"), user.altnick);
			}
		else
		{
			QPixmap *pix = NULL;
			switch (user.status) {
				case GG_STATUS_NOT_AVAIL_DESCR:
					if (has_mobile)
						pix = icons->loadIcon("offline_d_m");
					else
			    			pix = icons->loadIcon("offline_d");			
    		    			break;
				default:
					if (has_mobile)
						pix = icons->loadIcon("offline_m");
					else
		    				pix = icons->loadIcon("offline");			
		    			break;
				};
			if (pix != NULL)
				lbp = new KaduListBoxPixmap(*pix, user.altnick);
//				insertItem(*pix, user.altnick);
			else
				lbp = new KaduListBoxPixmap(*icons->loadIcon("online"), user.altnick);
//				insertItem(*icons->loadIcon("online"), user.altnick);
			insertItem(lbp);
		};
	};
	// Dodajemy uzytkownikow bez numerow GG
	for (i = 0; i < b_users.count(); i++) {
		UserListElement &user = userlist.byAltNick(b_users[i]);
		lbp = new KaduListBoxPixmap(*icons->loadIcon("mobile"), user.altnick);
		insertItem(lbp);
//		insertItem(*icons->loadIcon("mobile"), user.altnick);
		}
	// Przywracamy zaznaczenie wczesniej zaznaczonych uzytkownikow
	for (i = 0; i < s_users.count(); i++)
		setSelected(findItem(s_users[i]), true);
	setCurrentItem(findItem(s_user));

	kdebug("UserBox::refresh() exit\n");
};

void UserBox::addUser(const QString &altnick)
{
	Users.append(altnick);
};

void UserBox::removeUser(const QString &altnick)
{
	Users.remove(altnick);
};

void UserBox::renameUser(const QString &oldaltnick, const QString &newaltnick)
{
	QStringList::iterator it = Users.find(oldaltnick);
	(*it) = newaltnick;
};

void UserBox::changeAllToInactive()
{
	QPixmap *qp_inact = icons->loadIcon("offline");
	for(int i=0; i<count(); i++)
		changeItem(*qp_inact,item(i)->text(),i);
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

void UserBox::all_renameUser(const QString &oldaltnick, const QString &newaltnick)
{
	for(int i = 0; i < UserBoxes.size(); i++)
		UserBoxes[i]->renameUser(oldaltnick, newaltnick);	
};

QValueList<UserBox *> UserBox::UserBoxes;
