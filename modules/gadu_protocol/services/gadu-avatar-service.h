/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#ifndef GADU_AVATAR_SERVICE_H
#define GADU_AVATAR_SERVICE_H

#include "contacts/contact.h"
#include "protocols/services/avatar-service.h"

class GaduAvatarService : public AvatarService
{
	Q_OBJECT

public:
	GaduAvatarService(QObject *parent = 0) : AvatarService(parent) {}

	void fetchAvatar(Contact contact);

};

#endif // GADU_AVATAR_SERVICE_H
