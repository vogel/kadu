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

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "misc/kadu-paths.h"

#include "encryption-ng-otr-user-state-service.h"

#include "encryption-ng-otr-private-key-service.h"
#include <qfileinfo.h>
#include <QDir>

EncryptionNgOtrPrivateKeyService::EncryptionNgOtrPrivateKeyService(QObject *parent) :
		QObject(parent)
{
}

EncryptionNgOtrPrivateKeyService::~EncryptionNgOtrPrivateKeyService()
{
}

void EncryptionNgOtrPrivateKeyService::setEncryptionNgOtrUserStateService(EncryptionNgOtrUserStateService *encryptionNgOtrUserStateService)
{
	if (OtrUserStateService)
		disconnect(OtrUserStateService.data(), SIGNAL(userStateCreated(Account)), this, SLOT(userStateCreated(Account)));
		
	OtrUserStateService.reset(encryptionNgOtrUserStateService);

	if (OtrUserStateService)
	{
		connect(OtrUserStateService.data(), SIGNAL(userStateCreated(Account)), this, SLOT(userStateCreated(Account)));
		foreach (const Account &account, AccountManager::instance()->items())
			userStateCreated(account);
	}
}

void EncryptionNgOtrPrivateKeyService::userStateCreated(const Account &account)
{
	readPrivateKey(account);
}

QString EncryptionNgOtrPrivateKeyService::privateStoreFileName()
{
	return KaduPaths::instance()->profilePath() + QString("/keys/otr_private");
}

void EncryptionNgOtrPrivateKeyService::createPrivateKey(const Account &account)
{
	if (!OtrUserStateService)
		return;

	OtrlUserState userState = OtrUserStateService.data()->forAccount(account);
	QString fileName = privateStoreFileName();
	QFileInfo fileInfo(fileName);
	QDir fileDir = fileInfo.absoluteDir();
	if (!fileDir.exists())
	{
		printf("oo: %s\n", qPrintable(fileDir.absolutePath()));
		printf("aaaa: %d\n", fileDir.mkpath(fileDir.absolutePath()));
	}

	otrl_privkey_generate(userState, fileName.toUtf8().data(), account.id().toUtf8().data(), account.protocolName().toUtf8().data());
	readPrivateKey(account);
}

void EncryptionNgOtrPrivateKeyService::readPrivateKey(const Account &account)
{
	if (!OtrUserStateService)
		return;

	OtrlUserState userState = OtrUserStateService.data()->forAccount(account);
	otrl_privkey_read(userState, privateStoreFileName().toUtf8().data());
}
