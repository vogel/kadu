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

#include <QtCore/QDir>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "misc/kadu-paths.h"

#include "encryption-ng-otr-user-state.h"

#include "encryption-ng-otr-private-key-service.h"

EncryptionNgOtrPrivateKeyService::EncryptionNgOtrPrivateKeyService(QObject *parent) :
		QObject(parent), UserState(0)
{
}

EncryptionNgOtrPrivateKeyService::~EncryptionNgOtrPrivateKeyService()
{
}

void EncryptionNgOtrPrivateKeyService::setUserState(EncryptionNgOtrUserState *userState)
{
	UserState = userState;
}

QString EncryptionNgOtrPrivateKeyService::privateStoreFileName()
{
	return KaduPaths::instance()->profilePath() + QString("/keys/otr_private");
}

void EncryptionNgOtrPrivateKeyService::createPrivateKey(const Account &account)
{
	if (!UserState)
		return;

	OtrlUserState userState = UserState->userState();
	QString fileName = privateStoreFileName();
	QFileInfo fileInfo(fileName);
	QDir fileDir = fileInfo.absoluteDir();
	if (!fileDir.exists())
	{
		printf("oo: %s\n", qPrintable(fileDir.absolutePath()));
		printf("aaaa: %d\n", fileDir.mkpath(fileDir.absolutePath()));
	}

	otrl_privkey_generate(userState, fileName.toUtf8().data(), account.id().toUtf8().data(), account.protocolName().toUtf8().data());
	// readPrivateKeys(account);
}

void EncryptionNgOtrPrivateKeyService::readPrivateKeys()
{
	if (!UserState)
		return;

	OtrlUserState userState = UserState->userState();
	otrl_privkey_read(userState, privateStoreFileName().toUtf8().data());
}
