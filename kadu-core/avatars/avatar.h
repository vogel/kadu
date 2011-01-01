/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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
	static Avatar loadStubFromStorage(const QSharedPointer<StoragePoint> &storage);
	static Avatar loadFromStorage(const QSharedPointer<StoragePoint> &storage);
	static Avatar null;

	Avatar();
	Avatar(AvatarShared *data);
	Avatar(QObject *data);
	Avatar(const Avatar &copy);
	virtual ~Avatar();

	QString filePath();
	void setFilePath(const QString &filePath);

	KaduSharedBase_PropertyBoolRead(Empty)
	KaduSharedBase_Property(Buddy, avatarBuddy, AvatarBuddy)
	KaduSharedBase_Property(Contact, avatarContact, AvatarContact)
	KaduSharedBase_Property(QDateTime, lastUpdated, LastUpdated)
	KaduSharedBase_Property(QDateTime, nextUpdate, NextUpdate)
	KaduSharedBase_Property(QPixmap, pixmap, Pixmap)

};

Q_DECLARE_METATYPE(Avatar)

#endif // AVATAR_H
