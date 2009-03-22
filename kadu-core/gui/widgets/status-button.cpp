/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account.h"
#include "accounts/account_manager.h"
#include "protocols/protocol.h"
#include "status_changer.h"

#include "status-button.h"

StatusButton::StatusButton(QIcon icon, const QString& text, QWidget *parent) :
		QPushButton(icon, text, parent)
{
	connect(status_changer_manager, SIGNAL(statusChanged(Status)), this, SLOT(statusChanged(Status)));
}

void StatusButton::statusChanged(Status status)
{
	if (!AccountManager::instance()->defaultAccount())
		return;

	setIcon(AccountManager::instance()->defaultAccount()->statusPixmap(status));
	setText(Status::name(status));
}

void StatusButton::configurationUpdated()
{
	Account *account = AccountManager::instance()->defaultAccount();
	if (account)
		setIcon(account->protocol()->statusPixmap());
}
