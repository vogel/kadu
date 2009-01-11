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

class ContactsListWidgetDelegate;

class ContactsListWidget : public QListView
{
	ContactsListWidgetDelegate *Delegate;

public:
	ContactsListWidget(QWidget *parent = 0);
	virtual ~ContactsListWidget();

};

#endif // CONTACTS_LIST_WIDGET_H
