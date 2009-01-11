/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONTACTS_LIST_WIDGET_H
#define CONTACTS_LIST_WIDGET_H

#include <QtGui/QListView>

class QContextMenuEvent;
class QModelIndex;

class Contact;
class ContactList;
class ContactsListWidgetDelegate;
class KaduMainWindow;

class ContactsListWidget : public QListView
{
	KaduMainWindow *MainWindow;
	ContactsListWidgetDelegate *Delegate;

	Contact contact(const QModelIndex &index) const;

protected:
	virtual void contextMenuEvent(QContextMenuEvent *event);

public:
	ContactsListWidget(KaduMainWindow *mainWindow, QWidget *parent = 0);
	virtual ~ContactsListWidget();

	ContactList selectedContacts() const;

};

#endif // CONTACTS_LIST_WIDGET_H
