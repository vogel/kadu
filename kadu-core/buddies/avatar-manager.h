/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#ifndef AVATAR_MANAGER_H
#define AVATAR_MANAGER_H

#include <QtCore/QObject>
#include <QtGui/QPixmap>

#include "accounts/accounts-aware-object.h"
#include "buddies/avatar.h"
#include "configuration/storable-object.h"

class AvatarService;
class Contact;

class AvatarManager : public QObject, public StorableObject, AccountsAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(AvatarManager)

	static AvatarManager *Instance;

	QList<Avatar> Avatars;

	AvatarManager();
	virtual ~AvatarManager();

	AvatarService * avatarService(Account account);
	AvatarService * avatarService(Contact contact);

	QString avatarFileName(Avatar avatar);

private slots:
	void avatarFetched(Contact contact, const QByteArray &data);

protected:
	virtual StoragePoint * createStoragePoint();

	virtual void accountRegistered(Account account);
	virtual void accountUnregistered(Account account);

public:
	static AvatarManager * instance();

	virtual void load();
	virtual void store();

	void addAvatar(Avatar avatar);
	void removeAvatar(Avatar avatar);

	unsigned int count() { return Avatars.count(); }

	Avatar byIndex(unsigned int index);
	Avatar byUuid(const QString &uuid);

	void updateAvatar(Contact contact);

signals:
	void avatarAboutToBeAdded(Avatar avatar);
	void avatarAdded(Avatar avatar);
	void avatarAboutToBeRemoved(Avatar avatar);
	void avatarRemoved(Avatar avatar);

	void avatarUpdated(Contact contact);

};

// for MOC
#include "contacts/contact.h"

#endif // AVATAR_MANAGER_H
