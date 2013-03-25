/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "encryption-ng-otr-private-key-service.h"

#include "encryption-ng-otr-user-state-service.h"

EncryptionNgOtrUserStateService::EncryptionNgOtrUserStateService(QObject *parent) :
		QObject(parent)
{
	triggerAllAccountsRegistered();
}

EncryptionNgOtrUserStateService::~EncryptionNgOtrUserStateService()
{
	triggerAllAccountsUnregistered();
}
void EncryptionNgOtrUserStateService::accountRegistered(Account account)
{
	if (UserStates.contains(account))
		return;

	OtrlUserState userState = otrl_userstate_create();
	UserStates.insert(account, userState);

	emit userStateCreated(account);
}

void EncryptionNgOtrUserStateService::accountUnregistered(Account account)
{
	if (!UserStates.contains(account))
		return;

	OtrlUserState userState = UserStates.value(account);
	UserStates.remove(account);
	otrl_userstate_free(userState);
}

const QMap<Account, OtrlUserState> & EncryptionNgOtrUserStateService::userStates()
{
	return UserStates;
}

OtrlUserState EncryptionNgOtrUserStateService::forAccount(const Account &account)
{
	return UserStates.value(account);
}
