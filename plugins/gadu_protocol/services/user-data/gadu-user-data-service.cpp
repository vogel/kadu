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

#include "gadu-user-data-service.h"

#include "services/user-data/gadu-user-data-type.h"

#include "avatars/avatar-manager.h"
#include "contacts/contact-manager.h"

#include <libgadu.h>

GaduUserDataService::GaduUserDataService(Account account, QObject *parent) :
		AccountService{account, parent}
{
}

GaduUserDataService::~GaduUserDataService()
{
}

void GaduUserDataService::setAvatarManager(AvatarManager *avatarManager)
{
	m_avatarManager = avatarManager;
}

void GaduUserDataService::setContactManager(ContactManager *contactManager)
{
	m_contactManager = contactManager;
}

void GaduUserDataService::handleUserDataEvent(const gg_event_user_data &userData)
{
	auto update = userData.type == static_cast<int>(GaduUserDataType::Update);
	for (decltype(userData.user_count) i = 0; i < userData.user_count; i++)
		handleUserDataItem(userData.users[i], update);
}

void GaduUserDataService::handleUserDataItem(const gg_event_user_data_user &userDataUser, bool update)
{
	auto contact = m_contactManager->byId(account(), QString::number(userDataUser.uin), ActionReturnNull);
	if (!contact)
		return;

	auto gotAvatar = false;
	for (decltype(userDataUser.attr_count) i = 0; i < userDataUser.attr_count; i++)
	{
		auto key = QString::fromLatin1(userDataUser.attrs[i].key);
		if (key != "avatar")
			continue;

		auto ok = false;
		auto timestamp = QString::fromLatin1(userDataUser.attrs[i].value).toInt(&ok);

		if (userDataUser.attrs[i].type == 0 || !ok || timestamp <= 0)
			m_avatarManager->removeAvatar(contact);

		m_avatarManager->updateAvatar(contact, true);
		gotAvatar = true;
	}

	if (!update && !gotAvatar)
		m_avatarManager->removeAvatar(contact);
}

#include "moc_gadu-user-data-service.cpp"
