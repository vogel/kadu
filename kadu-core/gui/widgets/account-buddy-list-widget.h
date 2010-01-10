/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

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
	void buddiesListImported(bool ok, BuddyList buddies);
	void buddiesListExported(bool ok);

};

#endif // ACCOUNT_BUDDY_LIST_WIDGET_H
