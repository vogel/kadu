 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ADD_GROUP_WINDOW
#define ADD_GROUP_WINDOW

#include <QtGui/QWidget>

#include "contacts/contact-list.h"

class QLineEdit;
class QPushButton;

class AddGroupWindow : public QWidget
{
	Q_OBJECT

	ContactList contacts;

	QLineEdit *edit;
	QPushButton *okButton;

	void createGui();

private slots:
	void okClicked();
	void textChanged(const QString & groupName);

public:
	AddGroupWindow(QWidget *parent = 0);
	AddGroupWindow(ContactList &contactsToAdd, QWidget *parent = 0);

};

#endif // ADD_GROUP_WINDOW
