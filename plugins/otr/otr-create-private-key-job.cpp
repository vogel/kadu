/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

extern "C" {
#   include <libotr/privkey.h>
}

#include "accounts/account.h"

#include "otr-user-state.h"

#include "otr-create-private-key-job.h"

OtrCreatePrivateKeyJob::OtrCreatePrivateKeyJob(QObject *parent) :
		QObject(parent)
{
}

OtrCreatePrivateKeyJob::~OtrCreatePrivateKeyJob()
{
}

void OtrCreatePrivateKeyJob::setUserState(OtrUserState *userState)
{
	UserState = userState;
}

void OtrCreatePrivateKeyJob::setPrivateStoreFileName(const QString &privateStoreFileName)
{
	PrivateStoreFileName = privateStoreFileName;
}

void OtrCreatePrivateKeyJob::createPrivateKey(const Account &account)
{
	if (!UserState || PrivateStoreFileName.isEmpty())
	{
		emit finished(false);
		deleteLater();
		return;
	}

	OtrlUserState userState = UserState->userState();
	gcry_error_t err = otrl_privkey_generate(userState, PrivateStoreFileName.toUtf8().data(),
											 account.id().toUtf8().data(), account.protocolName().toUtf8().data());

	emit finished(0 == err);
	deleteLater();
}
