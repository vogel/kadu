/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include <QtWidgets/QAction>
#include <injeqt/injeqt.h>

class AccountManager;
class Actions;
class InjectedFactory;

class AddConferenceAction : public ActionDescription
{
	Q_OBJECT

public:
	explicit AddConferenceAction(Actions *actions, QObject *parent);
	virtual ~AddConferenceAction();

protected:
	virtual void triggered(QWidget *widget, ActionContext *context, bool toggled);

private:
	QPointer<AccountManager> m_accountManager;
	QPointer<Actions> m_actions;
	QPointer<InjectedFactory> m_injectedFactory;

private slots:
	INJEQT_SET void setAccountManager(AccountManager *accountManager);
	INJEQT_SET void setActions(Actions *actions);
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
	INJEQT_INIT void init();

	void updateAddChatMenuItem();

};
