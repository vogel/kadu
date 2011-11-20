/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "contacts/contact.h"

#include "key.h"

KaduSharedBaseClassImpl(Key)

Key Key::null;

Key Key::create()
{
	return new KeyShared();
}

Key Key::loadStubFromStorage(const QSharedPointer<StoragePoint> &avatarStoragePoint)
{
	return KeyShared::loadStubFromStorage(avatarStoragePoint);
}

Key Key::loadFromStorage(const QSharedPointer<StoragePoint> &avatarStoragePoint)
{
	return KeyShared::loadFromStorage(avatarStoragePoint);
}

Key::Key()
{
}

Key::Key(KeyShared *data) :
		SharedBase<KeyShared>(data)
{
}

Key::Key(QObject *data)
{
	KeyShared *shared = qobject_cast<KeyShared *>(data);
	if (shared)
		setData(shared);
}

Key::Key(const Key &copy) :
		SharedBase<KeyShared>(copy)
{
}

Key::~Key()
{
}

QString Key::filePath()
{
	return isNull() ? QString() : data()->filePath();
}

KaduSharedBase_PropertyBoolReadDef(Key, Empty, true)
KaduSharedBase_PropertyDefCRW(Key, QString, keyType, KeyType, QString())
KaduSharedBase_PropertyDefCRW(Key, Contact, keyContact, KeyContact, Contact::null)
KaduSharedBase_PropertyDefCRW(Key, QCA::SecureArray, key, Key, QCA::SecureArray())
