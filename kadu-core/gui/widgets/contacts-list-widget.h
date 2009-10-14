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

#include <QtGui/QWidget>

class QLineEdit;

class ContactNameFilter;
class ContactsListView;
class MainWindow;

class ContactsListWidget : public QWidget
{
	Q_OBJECT

	QLineEdit *NameFilterEdit;
	ContactsListView *View;
	ContactNameFilter *NameFilter;

private slots:
	void nameFilterChanged(const QString &filter);

public:
	explicit ContactsListWidget(MainWindow *mainWindow, QWidget *parent = 0);
	virtual ~ContactsListWidget();

	ContactsListView *view() { return View; }
	ContactNameFilter *nameFilter() { return NameFilter; }

};

#endif CONTACTS_LIST_WIDGET_H
