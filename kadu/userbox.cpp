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
//

MyListBox::MyListBox ( QWidget * parent, const char * name, WFlags f ) : QListBox(parent, name),
    QToolTip(viewport()) {
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

#include "userbox.moc"
