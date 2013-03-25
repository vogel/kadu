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

#ifndef ENCRYPTION_NG_OTR_USER_STATE_SERVICE_H
#define ENCRYPTION_NG_OTR_USER_STATE_SERVICE_H

#include <QtCore/QMap>
#include <QtCore/QObject>

#include "accounts/accounts-aware-object.h"

extern "C" {
#	include <libotr/userstate.h>
}

class EncryptionNgOtrUserStateService : public QObject, public AccountsAwareObject
{
	Q_OBJECT

	QMap<Account, OtrlUserState> UserStates;

	virtual void accountRegistered(Account account);
	virtual void accountUnregistered(Account account);

public:
	explicit EncryptionNgOtrUserStateService(QObject *parent = 0);
	virtual ~EncryptionNgOtrUserStateService();

	const QMap<Account, OtrlUserState> & userStates();
	OtrlUserState forAccount(const Account &account);

signals:
	void userStateCreated(const Account &account);

};


#endif // ENCRYPTION_NG_OTR_USER_STATE_SERVICE_H
