/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#ifndef AVATAR_SERVICE_H
#define AVATAR_SERVICE_H

#include <QtCore/QObject>
#include <QtGui/QPixmap>

#include "exports.h"

class Contact;

class KADUAPI AvatarService : public QObject
{
	Q_OBJECT

public:
	AvatarService(QObject *parent = 0) : QObject(parent) {}

	virtual void fetchAvatar(Contact *contactAccountData) = 0;

signals:
	void avatarFetched(Contact *contactAccountData, const QByteArray &avatar);

};

#endif // AVATAR_SERVICE_H
