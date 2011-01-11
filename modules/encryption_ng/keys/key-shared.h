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

#ifndef KEY_SHARED_H
#define KEY_SHARED_H

#include <QtCrypto>

#include "contacts/contact.h"

#include "storage/shared.h"
#include "../encryption_exports.h"

class ENCRYPTIONAPI KeyShared : public QObject, public Shared
{
	Q_OBJECT
	Q_DISABLE_COPY(KeyShared)

	QString KeyType;
	Contact KeyContact;
	QCA::SecureArray Key;

	QString KeysDir;

protected:
	virtual void load();

	virtual void emitUpdated();

public:
	static KeyShared * loadStubFromStorage(const QSharedPointer<StoragePoint> &avatarStoragePoint);
	static KeyShared * loadFromStorage(const QSharedPointer<StoragePoint> &avatarStoragePoint);

	explicit KeyShared(QUuid uuid = QUuid());
	virtual ~KeyShared();

	virtual StorableObject * storageParent();
	virtual QString storageNodeName();

	virtual void store();
	virtual bool shouldStore();
	virtual void aboutToBeRemoved();

	QString filePath();

	bool isEmpty();

	KaduShared_Property(QString, keyType, KeyType)
	KaduShared_Property(Contact, keyContact, KeyContact)
	KaduShared_Property(QCA::SecureArray, key, Key)

signals:
	void updated();

};

#endif // KEY_SHARED_H
