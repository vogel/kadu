/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "gui/actions/action-description.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class AccountManager;
class MenuInventory;

class ShowXmlConsoleActionDescription : public ActionDescription
{
	Q_OBJECT

protected:
	virtual void actionInstanceCreated(Action *action);
	virtual void actionTriggered(QAction *sender, bool toggled);

public:
	explicit ShowXmlConsoleActionDescription(QObject *parent);
	virtual ~ShowXmlConsoleActionDescription();

private:
	QPointer<Actions> m_actions;
	QPointer<AccountManager> m_accountManager;
	QPointer<MenuInventory> m_menuInventory;

private slots:
	INJEQT_SET void setAccountManager(AccountManager *accountManager);
	INJEQT_SET void setMenuInventory(MenuInventory *menuInventory);
	INJEQT_INIT void init();

	void insertMenuActionDescription();
	void updateShowXmlConsoleMenu();
	void menuActionTriggered(QAction *action);

};
