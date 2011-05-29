/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtGui/QImageReader>

#include "avatars/avatar-manager.h"
#include "misc/misc.h"

#include "avatar-shared.h"

AvatarShared * AvatarShared::loadStubFromStorage(const QSharedPointer<StoragePoint> &storagePoint)
{
	AvatarShared *result = loadFromStorage(storagePoint);
	result->loadStub();

	return result;
}

AvatarShared * AvatarShared::loadFromStorage(const QSharedPointer<StoragePoint> &storagePoint)
{
	AvatarShared *result = new AvatarShared();
	result->setStorage(storagePoint);

	return result;
}

AvatarShared::AvatarShared(const QUuid &uuid) :
		Shared(uuid)
{
	AvatarsDir = profilePath("avatars/");
}

AvatarShared::~AvatarShared()
{
	ref.ref();
}

StorableObject * AvatarShared::storageParent()
{
	return AvatarManager::instance();
}

QString AvatarShared::storageNodeName()
{
	return QLatin1String("Avatar");
}

QString AvatarShared::filePath()
{
	return FilePath.isEmpty() ? AvatarsDir + uuid().toString() : FilePath;
}

void AvatarShared::setFilePath(const QString &filePath)
{
	if (FilePath != filePath)
	{
		ensureLoaded();

		FilePath = filePath;

		QImageReader imageReader(filePath);
		Pixmap = QPixmap::fromImageReader(&imageReader);

		dataUpdated();
		emit pixmapUpdated();
	}
}

void AvatarShared::load()
{
	if (!isValidStorage())
		return;

	Shared::load();

	LastUpdated = loadValue<QDateTime>("LastUpdated");
	NextUpdate = loadValue<QDateTime>("NextUpdate");

	QImageReader imageReader(filePath());
	Pixmap = QPixmap::fromImageReader(&imageReader);
}

void AvatarShared::store()
{
	// do nothing. This dummy method avoid calling of ensureLoade on storing configuration
}

void AvatarShared::storeAvatar()
{
	if (!isValidStorage())
		return;

	ensureLoaded();

	Shared::store();

	storeValue("LastUpdated", LastUpdated);
	storeValue("NextUpdate", NextUpdate);

	QDir avatarsDir(profilePath("avatars"));
	if (!avatarsDir.exists())
		avatarsDir.mkpath(profilePath("avatars"));

	if (Pixmap.isNull())
		QFile::remove(filePath());
	else
		Pixmap.save(filePath(), "PNG");
}

bool AvatarShared::shouldStore()
{
	ensureLoaded();

	return UuidStorableObject::shouldStore() && !Pixmap.isNull();
}

void AvatarShared::aboutToBeRemoved()
{
	/* NOTE: This guard is needed to delay deleting this object when removing
	 * Avatar from Contact or Buddy holding last reference to it and thus wanting
	 * to delete it. But we don't want this to happen now because we have still
	 * some things to do here.
	 */
	Avatar guard(this);

	// cleanup references
	AvatarBuddy.setBuddyAvatar(Avatar::null);
	AvatarContact.setContactAvatar(Avatar::null);

	AvatarBuddy = Buddy::null;
	AvatarContact = Contact::null;

	QFile avatarFile(filePath());
	if (avatarFile.exists())
		avatarFile.remove();
}

bool AvatarShared::isEmpty()
{
	ensureLoaded();

	return Pixmap.isNull();
}

void AvatarShared::setPixmap(const QPixmap &pixmap)
{
	ensureLoaded();

	Pixmap = pixmap;
	dataUpdated();
	emit pixmapUpdated();
}

void AvatarShared::emitUpdated()
{
	emit updated();
}
