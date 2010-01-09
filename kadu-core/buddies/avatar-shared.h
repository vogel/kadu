/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef AVATAR_SHARED_H
#define AVATAR_SHARED_H

#include <QtCore/QDateTime>
#include <QtGui/QPixmap>

#include "contacts/contact.h"

#include "storage/shared.h"

class KADUAPI AvatarShared : public Shared
{
	Contact AvatarContact;
	QDateTime LastUpdated;
	QDateTime NextUpdate;
	QPixmap Pixmap;

	QString AvatarsDir;

protected:
	virtual void load();

public:
	static AvatarShared * loadFromStorage(StoragePoint *avatarStoragePoint);

	explicit AvatarShared(QUuid uuid = QUuid());
	virtual ~AvatarShared();

	virtual StorableObject * storageParent();
	virtual QString storageNodeName();

	virtual void store();
	virtual bool shouldStore();
	virtual void aboutToBeRemoved();

	QString filePath();

	bool isEmpty();

	void setPixmap(QPixmap pixmap);

	KaduShared_Property(Contact, avatarContact, AvatarContact)
	KaduShared_Property(QDateTime, lastUpdated, LastUpdated)
	KaduShared_Property(QDateTime, nextUpdate, NextUpdate)
	KaduShared_PropertyRead(QPixmap, pixmap, Pixmap)

};

#endif // AVATAR_SHARED_H
