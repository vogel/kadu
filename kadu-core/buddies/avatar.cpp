/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "buddies/avatar-shared.h"
#include "contacts/contact.h"
#include "misc/path-conversion.h"

#include "avatar.h"

Avatar Avatar::null(true);

Avatar Avatar::loadFromStorage(StoragePoint *contactStoragePoint)
{
	return Avatar(AvatarShared::loadFromStorage(contactStoragePoint));
}

Avatar::Avatar(bool null) :
		SharedBase<AvatarShared>(null)
{
}

Avatar::Avatar()
{
}

Avatar::Avatar(AvatarShared *data) :
		SharedBase<AvatarShared>(data)
{
}

Avatar::Avatar(const Avatar &copy) :
		SharedBase<AvatarShared>(copy)
{
}

Avatar::~Avatar()
{
}

Avatar & Avatar::operator = (const Avatar &copy)
{
	SharedBase<AvatarShared>::operator=(copy);
	return *this;
}

QString Avatar::filePath()
{
	return isNull() ? QString::null : data()->filePath();
}

KaduSharedBase_PropertyDef(Avatar, Contact, avatarContact, AvatarContact, Contact::null)
KaduSharedBase_PropertyDef(Avatar, QDateTime, lastUpdated, LastUpdated, QDateTime())
KaduSharedBase_PropertyDef(Avatar, QDateTime, nextUpdate, NextUpdate, QDateTime())
KaduSharedBase_PropertyDef(Avatar, QString, fileName, FileName, QString::null)
KaduSharedBase_PropertyDef(Avatar, QPixmap, pixmap, Pixmap, QPixmap())
