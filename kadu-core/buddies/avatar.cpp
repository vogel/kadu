/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "contacts/contact.h"
#include "misc/path-conversion.h"

#include "avatar.h"

Avatar::Avatar(Contact contact, bool loadFromConfiguration) :
		StorableObject("Avatar", contact.data()),
		MyContact(contact)
{
    	if (!loadFromConfiguration)
		StorableObject::setLoaded(true);
}

Avatar::~Avatar()
{
}

void Avatar::load()
{
	if (!isValidStorage())
		return;

	StorableObject::load();

	LastUpdated = loadValue<QDateTime>("LastUpdated");
	NextUpdate = loadValue<QDateTime>("NextUpdate");
	FileName = loadValue<QString>("FileName");

	QString avatarsPath = ggPath("avatars/");

	// TODO 0.6.6 - just remove this line
	FileName.remove(avatarsPath);
	FilePath = avatarsPath + FileName;

	Pixmap.load(avatarsPath + FileName);
}

void Avatar::store()
{
	if (!isValidStorage())
		return;

	ensureLoaded();

	storeValue("LastUpdated", LastUpdated);
	storeValue("NextUpdate", NextUpdate);
	storeValue("FileName", FileName);
}

Contact Avatar::contact()
{
	return MyContact;
}

QDateTime Avatar::lastUpdated()
{
	ensureLoaded();
	return LastUpdated;
}

void Avatar::setLastUpdated(const QDateTime &lastUpdated)
{
	ensureLoaded();
	LastUpdated = lastUpdated;
}

QDateTime Avatar::nextUpdate()
{
	ensureLoaded();
	return NextUpdate;
}

void Avatar::setNextUpdate(const QDateTime &nextUpdate)
{
	ensureLoaded();
	NextUpdate = nextUpdate;
}

QString Avatar::fileName()
{
	ensureLoaded();
	return FileName;
}

void Avatar::setFileName(const QString &fileName)
{
	ensureLoaded();
	FileName = fileName;

	QString avatarsPath = ggPath("avatars/");
	FilePath = avatarsPath + FileName;
}

QString Avatar::filePath()
{
    	ensureLoaded();
	return FilePath;
}

void Avatar::setFilePath(const QString &filePath)
{
	ensureLoaded();
	FilePath = filePath;
}

QPixmap Avatar::pixmap()
{
	ensureLoaded();
	return Pixmap;
}

void Avatar::setPixmap(const QPixmap &pixmap)
{
	ensureLoaded();
	Pixmap = pixmap;
}
