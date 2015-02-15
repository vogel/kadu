/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef CONTACT_PERSONAL_INFO_SERVICE_H
#define CONTACT_PERSONAL_INFO_SERVICE_H

#include "buddies/buddy.h"
#include "exports.h"

#include "protocols/services/account-service.h"

class KADUAPI ContactPersonalInfoService : public AccountService
{
	Q_OBJECT

public:
	explicit ContactPersonalInfoService(Account account, QObject *parent) : AccountService(account, parent) {}

	virtual void fetchPersonalInfo(Contact contact) = 0;

signals:
	void personalInfoAvailable(Buddy buddy);

};

#endif // CONTACT_PERSONAL_INFO_SERVICE_H
