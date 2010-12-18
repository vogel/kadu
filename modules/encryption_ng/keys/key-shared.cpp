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
#include <QtCore/QFile>

#include "keys/keys-manager.h"
#include "misc/misc.h"

#include "key-shared.h"

KeyShared * KeyShared::loadStubFromStorage(const QSharedPointer<StoragePoint> &storagePoint)
{
	KeyShared *result = loadFromStorage(storagePoint);
	result->loadStub();

	return result;
}

KeyShared * KeyShared::loadFromStorage(const QSharedPointer<StoragePoint> &storagePoint)
{
	KeyShared *result = new KeyShared();
	result->setStorage(storagePoint);

	return result;
}

KeyShared::KeyShared(QUuid uuid) :
		Shared(uuid)
{
	KeysDir = profilePath("keys/");
}

KeyShared::~KeyShared()
{
}

StorableObject * KeyShared::storageParent()
{
	return KeysManager::instance();
}

QString KeyShared::storageNodeName()
{
	return QLatin1String("Key");
}

QString KeyShared::filePath()
{
	ensureLoaded();
	return KeysDir + KeyType + "/" + uuid();
}

void KeyShared::load()
{
	if (!isValidStorage())
		return;

	Shared::load();

	KeyType = loadValue<QString>("KeyType");

	QFile keyFile(filePath());
	if (keyFile.exists() && keyFile.open(QFile::ReadOnly))
	{
		Key = keyFile.readAll();
		keyFile.close();
	}
}

void KeyShared::store()
{
	if (!isValidStorage())
		return;

	ensureLoaded();

	Shared::store();

	storeValue("KeyType", KeyType);

	QDir keysDir(profilePath("avatars/") + KeyType);
	if (!keysDir.exists())
		keysDir.mkpath(profilePath("avatars/") + KeyType);

	if (Key.isEmpty())
		QFile::remove(filePath());
	else
	{
		QFile keyFile(filePath());
		if (keyFile.open(QFile::WriteOnly))
		{
			keyFile.write(Key);
			keyFile.close();
		}
	}
}

bool KeyShared::shouldStore()
{
	return UuidStorableObject::shouldStore() && !Key.isEmpty();
}

void KeyShared::aboutToBeRemoved()
{
	QFile keyFile(filePath());
	if (keyFile.exists())
		keyFile.remove();
}

bool KeyShared::isEmpty()
{
	ensureLoaded();

	return Key.isEmpty();
}

void KeyShared::emitUpdated()
{
	emit updated();
}
