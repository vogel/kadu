/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "contacts/contact-manager.h"
#include "contacts/contact.h"
#include "misc/change-notifier.h"
#include "misc/kadu-paths.h"

#include "keys/keys-manager.h"

#include "key-shared.h"

KeyShared * KeyShared::loadStubFromStorage(const std::shared_ptr<StoragePoint> &storagePoint)
{
	KeyShared *result = loadFromStorage(storagePoint);
	result->loadStub();

	return result;
}

KeyShared * KeyShared::loadFromStorage(const std::shared_ptr<StoragePoint> &storagePoint)
{
	KeyShared *result = new KeyShared();
	result->setStorage(storagePoint);

	return result;
}

KeyShared::KeyShared(const QUuid &uuid) :
		Shared(uuid)
{
	KeysDir = KaduPaths::instance()->profilePath() + QLatin1String("keys/");
	KeyContact = new Contact();

	connect(&changeNotifier(), SIGNAL(changed()), this, SIGNAL(updated()));
}

KeyShared::~KeyShared()
{
	ref.ref();

	delete KeyContact;
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
	return KeysDir + KeyType + '/' + uuid().toString();
}

void KeyShared::load()
{
	if (!isValidStorage())
		return;

	Shared::load();

	KeyType = loadValue<QString>("KeyType");
	*KeyContact = ContactManager::instance()->byUuid(loadValue<QString>("Contact"));

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
	storeValue("Contact", KeyContact->uuid().toString());

	QDir keysDir(KeysDir + KeyType);
	if (!keysDir.exists())
	{
		keysDir.mkpath(KeysDir + KeyType);
		QFile::setPermissions(KeysDir, QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner);
		QFile::setPermissions(KeysDir + KeyType, QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner);
	}

	if (Key.isEmpty())
		QFile::remove(filePath());
	else
	{
		QFile keyFile(filePath());
		keyFile.setPermissions(QFile::ReadOwner | QFile::WriteOwner);

		if (keyFile.open(QFile::WriteOnly))
		{
			keyFile.write(Key.data());
			keyFile.close();
		}
	}
}

bool KeyShared::shouldStore()
{
	ensureLoaded();

	return UuidStorableObject::shouldStore() && !Key.isEmpty() && *KeyContact;
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

KaduShared_PropertyPtrDefCRW(KeyShared, Contact, keyContact, KeyContact)

#include "moc_key-shared.cpp"
