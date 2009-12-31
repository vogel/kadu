/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QDir>
#include <QtCore/QFile>

#include "buddies/avatar-manager.h"
#include "misc/misc.h"

#include "avatar-shared.h"

AvatarShared * AvatarShared::loadFromStorage(StoragePoint *storagePoint)
{
	AvatarShared *result = new AvatarShared();
	result->setStorage(storagePoint);
	result->load();

	return result;
}

AvatarShared::AvatarShared(QUuid uuid) :
		Shared(uuid), AvatarContact(Contact::null)
{
	AvatarsDir = ggPath("avatars/");
}

AvatarShared::~AvatarShared()
{
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
	ensureLoaded();
	return AvatarsDir + FileName;
}

void AvatarShared::load()
{
	if (!isValidStorage())
		return;

	Shared::load();

	LastUpdated = loadValue<QDateTime>("LastUpdated");
	NextUpdate = loadValue<QDateTime>("NextUpdate");
	FileName = loadValue<QString>("FileName");
	Pixmap.load(filePath());
}

void AvatarShared::store()
{
	if (!isValidStorage())
		return;

	ensureLoaded();

	Shared::store();

	storeValue("LastUpdated", LastUpdated);
	storeValue("NextUpdate", NextUpdate);
	storeValue("FileName", FileName);
}

void AvatarShared::aboutToBeRemoved()
{
	// cleanup references
	AvatarContact = Contact::null;

	QFile avatarFile(filePath());
	if (avatarFile.exists())
		avatarFile.remove();
}

bool AvatarShared::isEmpty() const
{
	return Pixmap.isNull();
}

void AvatarShared::setPixmap(QPixmap pixmap)
{
	QDir avatarsDir(ggPath("avatars"));
	if (!avatarsDir.exists())
		avatarsDir.mkpath(ggPath("avatars"));

	Pixmap = pixmap;

	if (pixmap.isNull())
		QFile::remove(avatarsDir.canonicalPath() + "/" + FileName);
	else
		pixmap.save(avatarsDir.canonicalPath() + "/" + FileName, "PNG");
}
