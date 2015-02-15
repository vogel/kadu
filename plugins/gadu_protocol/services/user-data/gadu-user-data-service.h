/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "protocols/services/account-service.h"

#include <QtCore/QPointer>

struct gg_event_user_data;
struct gg_event_user_data_user;

class AvatarManager;
class ContactManager;

class GaduUserDataService : public AccountService
{
	Q_OBJECT

public:
	explicit GaduUserDataService(Account account, QObject *parent = nullptr);
	virtual ~GaduUserDataService();

	void setAvatarManager(AvatarManager *avatarManager);
	void setContactManager(ContactManager *contactManager);

	void handleUserDataEvent(const gg_event_user_data &userData);

private:
	QPointer<AvatarManager> m_avatarManager;
	QPointer<ContactManager> m_contactManager;

	void handleUserDataItem(const gg_event_user_data_user &userDataUser, bool update);

};
