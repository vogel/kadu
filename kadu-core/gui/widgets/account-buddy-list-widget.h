/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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

#include "exports.h"

class BuddiesListWidget;
class ContactListService;

class KADUAPI AccountBuddyListWidget : public QWidget
{
	Q_OBJECT

	Account CurrentAccount;

	BuddiesListWidget *BuddiesWidget;

private slots:
	void restoreFromFile();
	void storeToFile();

public:
	explicit AccountBuddyListWidget(Account account, QWidget *parent = 0);
	virtual ~AccountBuddyListWidget() {}

};

#endif // ACCOUNT_BUDDY_LIST_WIDGET_H
