/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2010 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "core/core.h"
#include "gui/windows/kadu-window.h"

#include "kadurootobject.h"

KaduRootObject::KaduRootObject(QObject *parent) : QObject(parent)
{
}

KaduRootObject::~KaduRootObject()
{
}

QString KaduRootObject::Version()
{
	return "Kadu " + Core::version();
}

void KaduRootObject::Disconnect()
{
	AccountStatus.clear();
	foreach (Account account, AccountManager::instance()->items())
	{
		Status status = account.statusContainer()->status();
		if (status.isDisconnected())
			continue;

		AccountStatus.insert(account, status);
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

		account.statusContainer()->setStatus(AccountStatus.value(account));
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
