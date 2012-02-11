/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "accounts/account-manager.h"
#include "configuration/configuration-file.h"
#include "contacts/contact-manager.h"
#include "misc/kadu-paths.h"

#include "plugins/encryption_ng/keys/key.h"
#include "plugins/encryption_ng/keys/keys-manager.h"

#include "encryption-ng-simlite-key-importer.h"

EncryptioNgSimliteKeyImporter * EncryptioNgSimliteKeyImporter::Instance = 0;

void EncryptioNgSimliteKeyImporter::createInstance()
{
	Instance = new EncryptioNgSimliteKeyImporter();
}

void EncryptioNgSimliteKeyImporter::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

EncryptioNgSimliteKeyImporter::EncryptioNgSimliteKeyImporter()
{
	triggerAllAccountsRegistered();
}

void EncryptioNgSimliteKeyImporter::accountRegistered(Account account)
{
	if (account.id() == config_file.readEntry("General", "UIN"))
		importKeys(account);
}

void EncryptioNgSimliteKeyImporter::accountUnregistered(Account account)
{
	Q_UNUSED(account)
}

void EncryptioNgSimliteKeyImporter::importKey(const Account &account, const QFileInfo &fileInfo)
{
	if (!fileInfo.isReadable())
		return;

	QFile keyFile(fileInfo.filePath());
	if (!keyFile.open(QFile::ReadOnly))
		return;

	QByteArray keyData = keyFile.readAll();
	keyFile.close();

	QString fileName = fileInfo.fileName();
	QString keyName = fileName.left(fileName.length() - 4); // remove .pem suffix

	QString keyType = keyName == "private" ? "simlite_private" : "simlite";

	Contact contact = keyType == "simlite"
			? ContactManager::instance()->byId(account, keyName, ActionCreateAndAdd)
			: account.accountContact();

	if (!contact)
		return;

	Key key = KeysManager::instance()->byContactAndType(contact, keyType, ActionCreateAndAdd);
	key.setKey(keyData);
}

void EncryptioNgSimliteKeyImporter::importKeys(const Account &account)
{
	QString keysPath = KaduPaths::instance()->profilePath() + QLatin1String("keys/");

	QDir keysDir(keysPath);
	if (!keysDir.exists())
		return;

	QFileInfoList fileInfoList = keysDir.entryInfoList(QDir::Files);
	foreach (const QFileInfo &fileInfo, fileInfoList)
		importKey(account, fileInfo);

	KeysManager::instance()->ensureStored();
}
