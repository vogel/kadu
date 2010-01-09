/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "core/core.h"
#include "gui/windows/kadu-window.h"
#include "kadu-config.h"

#include "kadurootobject.h"

KaduRootObject::KaduRootObject(QObject *parent) : QObject(parent)
{
}

KaduRootObject::~KaduRootObject()
{
}

QString KaduRootObject::Version()
{
	return "Kadu " + Core::instance()->version();
}

void KaduRootObject::Disconnect()
{
	AccountStatus.clear();
	foreach (Account account, AccountManager::instance()->items())
	{
		Status status = account.statusContainer()->status();
		if (status.isDisconnected())
			continue;

		AccountStatus[account] = status;
		status.setType("Offline");
		account.statusContainer()->setStatus(status);
	}
}

void KaduRootObject::Reconnect()
{
	foreach (Account account, AccountManager::instance()->items())
	{
		if (!AccountStatus.contains(account))
			continue;

		account.statusContainer()->setStatus(AccountStatus[account]);
	}
}

void KaduRootObject::Show()
{
	Core::instance()->kaduWindow()->show();
}

void KaduRootObject::Hide()
{
	Core::instance()->kaduWindow()->hide();
}

void KaduRootObject::Quit()
{
	Core::instance()->quit();
}
