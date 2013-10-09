/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef AVATAR_MANAGER_H
#define AVATAR_MANAGER_H

#include <QtCore/QObject>

#include "accounts/accounts-aware-object.h"
#include "avatars/avatar.h"
#include "storage/simple-manager.h"
#include "exports.h"

class AvatarService;
class Buddy;
class Contact;

class KADUAPI AvatarManager : public QObject, public SimpleManager<Avatar>, AccountsAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(AvatarManager)

	static AvatarManager *Instance;

	QTimer *UpdateTimer;

	AvatarManager();
	virtual ~AvatarManager();

	void init();

	bool needUpdate(const Contact &contact);

private slots:
	void avatarDataUpdated();
	void avatarPixmapUpdated();

	void updateAvatars();
	void updateAccountAvatars();
	void contactAdded(Contact contact);

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

	Avatar byBuddy(Buddy buddy, NotFoundAction action);
	Avatar byContact(Contact contact, NotFoundAction action);

	void updateAvatar(const Contact &contact, bool force = false);

signals:
	void avatarAboutToBeAdded(Avatar avatar);
	void avatarAdded(Avatar avatar);
	void avatarAboutToBeRemoved(Avatar avatar);
	void avatarRemoved(Avatar avatar);

	void avatarUpdated(Avatar avatar);

};

#endif // AVATAR_MANAGER_H
