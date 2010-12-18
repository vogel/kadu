/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "misc/path-conversion.h"

#include "modules/encryption_ng/keys/keys-manager.h"
#include "modules/encryption_ng/keys/key.h"

#include "key-importer.h"

void EncryptioNgSimlite::importKey(const QFileInfo &fileInfo)
{
	if (!fileInfo.isReadable())
		return;

	QFile keyFile(fileInfo.filePath());
	if (!keyFile.open(QFile::ReadOnly))
		return;

	QByteArray keyData = keyFile.readAll();
	keyFile.close();


	QString fileName = fileInfo.fileName();
	QString keyType = fileName == "private.pem" ? "simlite_private" : "simlite";

	Key key = Key::create();
	key.setKey(keyData);
	key.setKeyType(keyType);

	KeysManager::instance()->addItem(key);
}

void EncryptioNgSimlite::importKeys()
{
	QString keysPath = profilePath("keys/");

	QDir keysDir(keysPath);
	if (!keysDir.exists())
		return;

	QFileInfoList fileInfoList = keysDir.entryInfoList(QDir::Files);
	foreach (const QFileInfo &fileInfo, fileInfoList)
		importKey(fileInfo);

	KeysManager::instance()->store();
}
