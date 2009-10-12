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
#include "contacts/contact-list.h"
#include "gui/widgets/contacts-list-view.h"

#include "exports.h"

class KADUAPI AccountContactsListWidget : public QWidget
{
	Q_OBJECT
	
	Account *CurrentAccount;
	ContactsListView *ContactsWidget;
	QPushButton *ExportButton;
	QPushButton *ImportButton;
	bool Clear;

public:
	AccountContactsListWidget(Account *account, QWidget *parent = 0);
	virtual ~AccountContactsListWidget() {}

private slots:
	void startImportTransfer();
	void startExportTransfer();
	void contactListImported(bool ok, ContactList contacts);
	void contactListExported(bool ok);

};

#endif // ACCOUNT_CONTACTS_LIST_WIDGET_H
