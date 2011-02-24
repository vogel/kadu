/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef AVATAR_SHARED_H
#define AVATAR_SHARED_H

#include <QtCore/QDateTime>
#include <QtGui/QPixmap>

#include "contacts/contact.h"

#include "storage/shared.h"

class KADUAPI AvatarShared : public QObject, public Shared
{
	Q_OBJECT
	Q_DISABLE_COPY(AvatarShared)

	Buddy AvatarBuddy;
	Contact AvatarContact;
	QDateTime LastUpdated;
	QDateTime NextUpdate;
	QString FilePath;
	QPixmap Pixmap;

	QString AvatarsDir;

protected:
	virtual void load();

	virtual void emitUpdated();

public:
	static AvatarShared * loadStubFromStorage(const QSharedPointer<StoragePoint> &avatarStoragePoint);
	static AvatarShared * loadFromStorage(const QSharedPointer<StoragePoint> &avatarStoragePoint);

	explicit AvatarShared(QUuid uuid = QUuid());
	virtual ~AvatarShared();

	virtual StorableObject * storageParent();
	virtual QString storageNodeName();

	virtual void store();
	virtual bool shouldStore();
	virtual void aboutToBeRemoved();

	QString filePath();
	void setFilePath(const QString &filePath);

	bool isEmpty();

	void setPixmap(QPixmap pixmap);

	KaduShared_Property(Buddy, avatarBuddy, AvatarBuddy)
	KaduShared_Property(Contact, avatarContact, AvatarContact)
	KaduShared_Property(QDateTime, lastUpdated, LastUpdated)
	KaduShared_Property(QDateTime, nextUpdate, NextUpdate)
	KaduShared_PropertyRead(QPixmap, pixmap, Pixmap)

signals:
	void updated();
	void pixmapUpdated();

};

#endif // AVATAR_SHARED_H
