/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010, 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef AVATAR_H
#define AVATAR_H

#include <QtCore/QDateTime>
#include <QtGui/QPixmap>

#include "avatars/avatar-shared.h"
#include "storage/shared-base.h"
#include "exports.h"

class Contact;
class StoragePoint;

class KADUAPI Avatar : public SharedBase<AvatarShared>
{
	KaduSharedBaseClass(Avatar)

public:
	static Avatar create();
	static Avatar loadStubFromStorage(const std::shared_ptr<StoragePoint> &storage);
	static Avatar loadFromStorage(const std::shared_ptr<StoragePoint> &storage);
	static Avatar null;

	Avatar();
	Avatar(AvatarShared *data);
	explicit Avatar(QObject *data);
	Avatar(const Avatar &copy);
	virtual ~Avatar();

	virtual void storeAvatar();

	KaduSharedBase_PropertyBoolRead(Empty)
	KaduSharedBase_PropertyCRW(QDateTime, lastUpdated, LastUpdated)
	KaduSharedBase_PropertyCRW(QDateTime, nextUpdate, NextUpdate)
	KaduSharedBase_PropertyCRW(QPixmap, pixmap, Pixmap)
	KaduSharedBase_PropertyRead(QString, smallFilePath, SmallFilePath)
	KaduSharedBase_PropertyCRW(QString, filePath, FilePath)

};

Q_DECLARE_METATYPE(Avatar)

#endif // AVATAR_H
