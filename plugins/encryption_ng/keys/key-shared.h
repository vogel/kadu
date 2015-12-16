/*
 * %kadu copyright begin%
 * Copyright 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef KEY_SHARED_H
#define KEY_SHARED_H

#include <QtCore/QDataStream>
#include <QtCrypto>

#include "../encryption_exports.h"
#include "storage/shared.h"

class Contact;

class ENCRYPTIONAPI KeyShared : public QObject, public Shared
{
	Q_OBJECT
	Q_DISABLE_COPY(KeyShared)

	QString KeyType;
	Contact *KeyContact;
	QCA::SecureArray Key;

	QString KeysDir;

protected:
	virtual void load();
	virtual void store();
	virtual bool shouldStore();

public:
	static KeyShared * loadStubFromStorage(const std::shared_ptr<StoragePoint> &avatarStoragePoint);
	static KeyShared * loadFromStorage(const std::shared_ptr<StoragePoint> &avatarStoragePoint);

	explicit KeyShared(const QUuid &uuid = QUuid());
	virtual ~KeyShared();

	virtual StorableObject * storageParent();
	virtual QString storageNodeName();

	virtual void aboutToBeRemoved();

	QString filePath();

	bool isEmpty();

	KaduShared_PropertyDeclCRW(Contact, keyContact, KeyContact)
	KaduShared_Property(const QString &, keyType, KeyType)
	KaduShared_Property(const QCA::SecureArray &, key, Key)

signals:
	void updated();

};

#endif // KEY_SHARED_H
