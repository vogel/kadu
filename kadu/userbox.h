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
#include <qstringlist.h>
#include "../libgadu/lib/libgadu.h"

class UserBox : public QListBox , QToolTip
{    
	Q_OBJECT

	private:
		static QValueList<UserBox*> UserBoxes;
		QStringList Users;

		void sortUsersByAltNick(QStringList &);
			
	protected:
		virtual void maybeTip(const QPoint&);
		virtual void mousePressEvent(QMouseEvent *e);

	public:
		UserBox(QWidget* parent=0,const char* name=0,WFlags f=0);
		~UserBox();
		virtual void clear() { QListBox::clear(); };
		void clearUsers() { Users.clear(); };
		void refresh();
		void addUser(const QString &altnick);
		void removeUser(const QString &altnick);
		void renameUser(const QString &oldaltnick, const QString &newaltnick);
		void changeAllToInactive();
		// Functions below works on all created userboxes
		static void all_refresh();
		static void all_removeUser(QString &altnick);		
		static void all_changeAllToInactive();
		static void all_renameUser(const QString &oldaltnick, const QString &newaltnick);		
};

#endif
