/***************************************************************************
                          userbox.h -  description
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

#ifndef USERBOX_H
#define USERBOX_H

#include <qlistbox.h>
#include <qtooltip.h>
#include "../libgadu/lib/libgadu.h"

class MyListBox : public QListBox , QToolTip {
    Q_OBJECT
    public:
	MyListBox ( QWidget * parent=0, const char * name=0, WFlags f=0 );
	virtual void clear() { QListBox::clear(); };

    protected:
	virtual void maybeTip(const QPoint&);
	int descriptionForUser(QListBoxItem *);
};

#endif
