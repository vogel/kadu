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

class UserBox : public QListBox , QToolTip
{    
	Q_OBJECT

	private:
		static QValueList<UserBox*> UserBoxes;
		QValueList<uin_t> Uins;
	
	protected:
		virtual void maybeTip(const QPoint&);
		int descriptionForUser(QListBoxItem *);
		int findUinInUserlist(uin_t uin);    
		int findCommentInUserlist(QString& comment);
		void sortUsers();
		
	public:
		UserBox(QWidget* parent=0,const char* name=0,WFlags f=0);
		~UserBox();
		virtual void clear() { QListBox::clear(); };
		void refresh();
		void addUin(uin_t uin);
		void removeUin(uin_t uin);
		void removeUser(QString& username);
		void changeUserStatus(
		void changeAllToInactive();
		// Functions below works on all created userboxes
		static void all_refresh();
		static void all_removeUser(QString& username);		
		static void all_changeAllToInactive();		
};

#endif
