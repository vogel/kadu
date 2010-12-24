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

#ifndef KEY_H
#define KEY_H

#include "key-shared.h"

#include "storage/shared-base.h"
#include "exports.h"

class Contact;
class StoragePoint;

class KADUAPI Key : public SharedBase<KeyShared>
{
	KaduSharedBaseClass(Key)

public:
	static Key create();
	static Key loadStubFromStorage(const QSharedPointer<StoragePoint> &storage);
	static Key loadFromStorage(const QSharedPointer<StoragePoint> &storage);
	static Key null;

	Key();
	Key(KeyShared *data);
	Key(QObject *data);
	Key(const Key &copy);
	virtual ~Key();

	QString filePath();

	KaduSharedBase_PropertyBoolRead(Empty)
	KaduSharedBase_Property(QString, keyType, KeyType)
	KaduSharedBase_Property(Contact, keyContact, KeyContact)
	KaduSharedBase_Property(QCA::SecureArray, key, Key)

};

Q_DECLARE_METATYPE(Key)

#endif // KEY_H
