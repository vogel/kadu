/*
 * %kadu copyright begin%
 * Copyright 2011 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2008, 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
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

#include "../encryption_exports.h"
#include "storage/shared-base.h"

class Contact;
class StoragePoint;

class ENCRYPTIONAPI Key : public SharedBase<KeyShared>
{
	KaduSharedBaseClass(Key)

public:
	static Key create();
	static Key loadStubFromStorage(const std::shared_ptr<StoragePoint> &storage);
	static Key loadFromStorage(const std::shared_ptr<StoragePoint> &storage);
	static Key null;

	Key();
	Key(KeyShared *data);
	explicit Key(QObject *data);
	Key(const Key &copy);
	virtual ~Key();

	QString filePath();

	KaduSharedBase_PropertyBoolRead(Empty)
	KaduSharedBase_PropertyCRW(QString, keyType, KeyType)
	KaduSharedBase_PropertyCRW(Contact, keyContact, KeyContact)
	KaduSharedBase_PropertyCRW(QCA::SecureArray, key, Key)

};

Q_DECLARE_METATYPE(Key)

#endif // KEY_H
