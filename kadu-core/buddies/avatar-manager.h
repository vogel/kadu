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
#include "storage/simple-manager.h"

class AvatarService;
class Contact;

class AvatarManager : public QObject, public SimpleManager<Avatar>, AccountsAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(AvatarManager)

	static AvatarManager *Instance;

	QTimer *UpdateTimer;

	AvatarManager();
	virtual ~AvatarManager();

	AvatarService * avatarService(Account account);
	AvatarService * avatarService(Contact contact);

	QString avatarFileName(Avatar avatar);

	bool needUpdate(Contact contact);
	void updateAvatar(Contact contact, bool force = false);

private slots:
	void avatarFetched(Contact contact, const QByteArray &data);
	void updateAvatars();
	void updateAccountAvatars();

protected:
	virtual void accountRegistered(Account account);
	virtual void accountUnregistered(Account account);

	virtual void itemAboutToBeAdded(Avatar item);
	virtual void itemAdded(Avatar item);
	virtual void itemAboutToBeRemoved(Avatar item);
	virtual void itemRemoved(Avatar item);

public:
	static AvatarManager * instance();

	virtual QString storageNodeName() { return QLatin1String("Avatars"); }
	virtual QString storageNodeItemName() { return QLatin1String("Avatar"); }

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
