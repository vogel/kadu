/*
 * %kadu copyright begin%
 * Copyright 2016 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "buddy-dummy-factory.h"

#include "accounts/account-storage.h"
#include "accounts/account.h"
#include "avatars/avatar-manager.h"
#include "buddies/buddy-storage.h"
#include "buddies/buddy.h"
#include "contacts/contact.h"
#include "core/injected-factory.h"
#include "icons/icons-manager.h"
#include "icons/kadu-icon.h"
#include "identities/identity.h"
#include "status/status-type-manager.h"

#include <QtWidgets/QApplication>

BuddyDummyFactory::BuddyDummyFactory(QObject *parent) :
		QObject{parent}
{
}

BuddyDummyFactory::~BuddyDummyFactory()
{
}

void BuddyDummyFactory::setAccountStorage(AccountStorage *accountStorage)
{
	m_accountStorage = accountStorage;
}

void BuddyDummyFactory::setAvatarManager(AvatarManager *avatarManager)
{
	m_avatarManager = avatarManager;
}

void BuddyDummyFactory::setBuddyStorage(BuddyStorage *buddyStorage)
{
	m_buddyStorage = buddyStorage;
}

void BuddyDummyFactory::setIconsManager(IconsManager *iconsManager)
{
	m_iconsManager = iconsManager;
}

void BuddyDummyFactory::setInjectedFactory(InjectedFactory *injectedFactory)
{
	m_injectedFactory = injectedFactory;
}

void BuddyDummyFactory::setStatusTypeManager(StatusTypeManager *statusTypeManager)
{
	m_statusTypeManager = statusTypeManager;
}

Buddy BuddyDummyFactory::dummy()
{
	auto example = m_buddyStorage->create();

	example.setFirstName("Mark");
	example.setLastName("Smith");
	example.setNickName("Jimbo");
	example.setDisplay("Jimbo");
	example.setMobile("+48123456789");
	example.setEmail("jimbo@mail.server.net");
	example.setHomePhone("+481234567890");

	Identity identity = Identity::create();
	identity.setName(QApplication::translate("Buddy", "Example identity"));

	Account account = m_accountStorage->create("");
	account.setAccountIdentity(identity);

	Contact contact = Contact::create();
	contact.setContactAccount(account);
	contact.setOwnerBuddy(example);
	contact.setId("999999");
	contact.setCurrentStatus(Status(m_statusTypeManager, StatusTypeAway, QApplication::translate("Buddy", "Example description")));

	// this is just an example contact, do not add avatar to list
	auto avatar = m_avatarManager->byContact(contact, ActionCreate);

	avatar.setLastUpdated(QDateTime::currentDateTime());
	avatar.setFilePath(m_iconsManager->iconPath(KaduIcon("kadu_icons/buddy0", "96x96")));

	example.addContact(contact);
	example.setAnonymous(false);

	return example;
}

#include "moc_buddy-dummy-factory.cpp"
