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
class ContactAccountData;

class AvatarManager : public QObject, public AccountsAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(AvatarManager)

	static AvatarManager *Instance;

	AvatarManager();
	virtual ~AvatarManager();

	AvatarService * avatarService(Account *account);
	AvatarService * avatarService(ContactAccountData *contactAccountData);

	QString avatarFileName(Avatar avatar);

private slots:
	void avatarFetched(ContactAccountData *contactAccountData, const QByteArray &data);

protected:
	virtual void accountRegistered(Account *account);
	virtual void accountUnregistered(Account *account);

public:
	static AvatarManager * instance();

	void updateAvatar(ContactAccountData *contactAccountData);

signals:
	void avatarUpdated(ContactAccountData *contactAccountData);

};

#endif // AVATAR_MANAGER_H
