/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ACCOUNT_BUDDY_LIST_WIDGET_H
#define ACCOUNT_BUDDY_LIST_WIDGET_H

#include <QtGui/QPushButton>
#include <QtGui/QWidget>

#include "accounts/account.h"
#include "buddies/buddy-list.h"
#include "gui/widgets/buddies-list-view.h"

#include "exports.h"

class KADUAPI AccountBuddyListWidget : public QWidget
{
	Q_OBJECT
	
	Account CurrentAccount;
	BuddiesListView *BuddiesWidget;
	QPushButton *ExportButton;
	QPushButton *ImportButton;
	bool Clear;

public:
	AccountBuddyListWidget(Account account, QWidget *parent = 0);
	virtual ~AccountBuddyListWidget() {}

private slots:
	void startImportTransfer();
	void startExportTransfer();
	void buddiesListImported(bool ok, BuddyList contacts);
	void buddiesListExported(bool ok);

};

#endif // ACCOUNT_BUDDY_LIST_WIDGET_H
