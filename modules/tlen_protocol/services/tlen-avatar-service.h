/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#ifndef TLEN_AVATAR_SERVICE_H
#define TLEN_AVATAR_SERVICE_H

#include <QList>

#include "contacts/contact.h"
#include "protocols/services/avatar-service.h"

class TlenAvatarService : public AvatarService
{
	Q_OBJECT

	QList<Contact *> InProgress;

public:
	TlenAvatarService(QObject *parent = 0) : AvatarService(parent) {}

	void fetchAvatar(Contact *contactAccountData);

private slots:
	void avatarReady(Contact *contactAccountData, const QByteArray &avatar);

};

#endif // TLEN_AVATAR_SERVICE_H
