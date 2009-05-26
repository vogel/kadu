/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ACCOUNT_CONTACTS_LIST_WIDGET_H
#define ACCOUNT_CONTACTS_LIST_WIDGET_H

#include <QtGui/QPushButton>
#include <QtGui/QWidget>

#include "accounts/account.h"
#include "gui/widgets/contacts-list-widget.h"

class AccountContactsListWidget : public QWidget
{
	Q_OBJECT
	
	Account *CurrentAccount;
	ContactsListWidget *ContactsWidget;
	QPushButton *ExportButton;
	QPushButton *ImportButton;

public:
	AccountContactsListWidget(Account *account, QWidget *parent = 0);
	virtual ~AccountContactsListWidget() {}

};

#endif // ACCOUNT_CONTACTS_LIST_WIDGET_H
