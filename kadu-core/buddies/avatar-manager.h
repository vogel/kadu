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

class Avatar;
class AvatarService;
class Contact;

class AvatarManager : public QObject, public AccountsAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(AvatarManager)

	static AvatarManager *Instance;

	AvatarManager();
	virtual ~AvatarManager();

	AvatarService * avatarService(Account account);
	AvatarService * avatarService(Contact contact);

	QString avatarFileName(Avatar avatar);

private slots:
	void avatarFetched(Contact contact, const QByteArray &data);

protected:
	virtual void accountRegistered(Account account);
	virtual void accountUnregistered(Account account);

public:
	static AvatarManager * instance();

	void updateAvatar(Contact contact);

signals:
	void avatarUpdated(Contact contact);

};

// for MOC
#include "contacts/contact.h"

#endif // AVATAR_MANAGER_H
