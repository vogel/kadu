/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtGui/QImageReader>

#include "avatars/avatar-manager.h"
#include "buddies/buddy.h"
#include "contacts/contact.h"
#include "core/application.h"
#include "misc/change-notifier.h"
#include "misc/paths-provider.h"

#include "avatar-shared.h"

AvatarShared * AvatarShared::loadStubFromStorage(const std::shared_ptr<StoragePoint> &storagePoint)
{
	AvatarShared *result = loadFromStorage(storagePoint);
	result->loadStub();

	return result;
}

AvatarShared * AvatarShared::loadFromStorage(const std::shared_ptr<StoragePoint> &storagePoint)
{
	AvatarShared *result = new AvatarShared();
	result->setStorage(storagePoint);

	return result;
}

AvatarShared::AvatarShared(const QUuid &uuid) :
		Shared(uuid)
{
	AvatarsDir = Application::instance()->pathsProvider()->profilePath() + QLatin1String("avatars/");

	connect(&changeNotifier(), SIGNAL(changed()), this, SIGNAL(updated()));
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
	return FilePath.isEmpty() && !uuid().toString().isEmpty() ? AvatarsDir + uuid().toString() : FilePath;
}

QString AvatarShared::smallFilePath()
{
	return SmallFilePath.isEmpty() ? filePath() : SmallFilePath;
}

void AvatarShared::ensureSmallPixmapExists()
{
	QFileInfo file(filePathToSmallFilePath(filePath()));
	if (!file.exists())
		storeSmallPixmap();
}

void AvatarShared::setFilePath(const QString &filePath)
{
	if (FilePath != filePath)
	{
		ensureLoaded();

		FilePath = filePath;

		QImageReader imageReader(filePath);
		Pixmap = QPixmap::fromImageReader(&imageReader);

		ensureSmallPixmapExists();

		changeNotifier().notify();
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

	QFileInfo avatarFile(filePath());

	if (avatarFile.exists() && avatarFile.isReadable() && avatarFile.isFile())
	{
		QImageReader imageReader(avatarFile.filePath());
		Pixmap = QPixmap::fromImageReader(&imageReader);
	}

	ensureSmallPixmapExists();
}

void AvatarShared::store()
{
	// do nothing. This dummy method avoid calling of ensureLoaded on storing configuration
}

void AvatarShared::storeAvatar()
{
	if (!isValidStorage())
		return;

	Shared::store();

	storeValue("LastUpdated", LastUpdated);
	storeValue("NextUpdate", NextUpdate);

	QDir avatarsDir(Application::instance()->pathsProvider()->profilePath() + QLatin1String("avatars"));
	if (!avatarsDir.exists())
		avatarsDir.mkpath(QLatin1String("."));

	if (Pixmap.isNull())
		QFile::remove(filePath());
	else
		Pixmap.save(filePath(), "PNG");

	storeSmallPixmap();
}

QString AvatarShared::filePathToSmallFilePath(const QString &filePath)
{
	return filePath + "-small";
}

bool AvatarShared::isPixmapSmall()
{
	if (Pixmap.isNull())
		return false;

	return Pixmap.width() <= 128 && Pixmap.height() <= 128;
}

void AvatarShared::storeSmallPixmap()
{
	if (!isValidStorage())
		return;

	QDir avatarsDir(Application::instance()->pathsProvider()->profilePath() + QLatin1String("avatars"));
	if (!avatarsDir.exists())
		avatarsDir.mkpath(QLatin1String("."));

	SmallFilePath = filePathToSmallFilePath(filePath());

	if (Pixmap.isNull() || isPixmapSmall())
	{
		QFile::remove(SmallFilePath);
		SmallFilePath.clear();
		return;
	}

	QPixmap smallPixmap = Pixmap.scaled(128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	smallPixmap.save(SmallFilePath, "PNG");
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
	changeNotifier().notify();
	emit pixmapUpdated();
}

#include "moc_avatar-shared.cpp"
