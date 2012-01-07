/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009, 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "accounts/account.h"
#include "avatars/avatar-manager.h"
#include "avatars/avatar.h"
#include "buddies/buddy-manager.h"
#include "configuration/configuration-manager.h"
#include "configuration/xml-configuration-file.h"
#include "contacts/contact.h"
#include "core/core.h"
#include "icons/kadu-icon.h"
#include "identities/identity.h"
#include "protocols/protocol.h"
#include "protocols/protocols-manager.h"

#include "buddy.h"

KaduSharedBaseClassImpl(Buddy)

Buddy Buddy::null;

Buddy Buddy::create()
{
	return new BuddyShared();
}

Buddy Buddy::loadStubFromStorage(const QSharedPointer<StoragePoint> &contactStoragePoint)
{
	return BuddyShared::loadStubFromStorage(contactStoragePoint);
}

Buddy Buddy::loadFromStorage(const QSharedPointer<StoragePoint> &contactStoragePoint)
{
	return BuddyShared::loadFromStorage(contactStoragePoint);
}

Buddy::Buddy()
{
}

Buddy::Buddy(BuddyShared *data) :
		SharedBase<BuddyShared>(data)
{
}

Buddy::Buddy(QObject *data)
{
	BuddyShared *shared = qobject_cast<BuddyShared *>(data);
	if (shared)
		setData(shared);
}

Buddy::Buddy(const Buddy &copy) :
		SharedBase<BuddyShared>(copy)
{
}

Buddy::~Buddy()
{
	if (data())
		data()->collectGarbage();
}

void Buddy::importConfiguration(const QDomElement &parent)
{
	if (data())
		data()->importConfiguration(parent);
}

void Buddy::importConfiguration()
{
	if (data())
		data()->importConfiguration();
}

QString Buddy::customData(const QString &key)
{
	return isNull() ? QString() : data()->customData()[key];
}

void Buddy::setCustomData(const QString &key, const QString &value)
{
	if (!isNull())
		data()->customData().insert(key, value);
}

void Buddy::removeCustomData(const QString &key)
{
	if (!isNull())
		data()->customData().remove(key);
}

void Buddy::sortContacts()
{
	if (!isNull())
		data()->sortContacts();
}

void Buddy::normalizePriorities()
{
	if (!isNull())
		data()->normalizePriorities();
}

void Buddy::addContact(Contact contact)
{
	if (isNull() || contact.isNull())
		return;

	data()->addContact(contact);
}

void Buddy::removeContact(Contact contact) const
{
	if (!isNull())
		data()->removeContact(contact);
}

QVector<Contact> Buddy::contacts(Account account) const
{
	return isNull() ? QVector<Contact>() : data()->contacts(account);
}

QList<Contact> Buddy::contacts() const
{
	return isNull() ? QList<Contact>() : data()->contacts();
}

bool Buddy::hasContact(Account account) const
{
	return isNull() ? false : !data()->contacts(account).isEmpty();
}

QString Buddy::id(Account account) const
{
	return isNull() ? QString() : data()->id(account);
}

bool Buddy::isInGroup(Group group) const
{
	return isNull() ? false : data()->isInGroup(group);
}

bool Buddy::showInAllGroup() const
{
	return isNull() ? false : data()->showInAllGroup();
}

void Buddy::addToGroup(Group group) const
{
	if (!isNull() && !data()->isInGroup(group))
		data()->addToGroup(group);

}
void Buddy::removeFromGroup(Group group) const
{
	if (!isNull() && data()->isInGroup(group))
		data()->removeFromGroup(group);
}

bool Buddy::isEmpty(bool checkOnlyForContacts) const
{
	if (isNull())
		return true;
	else
		return data()->isEmpty(checkOnlyForContacts);
}

QString Buddy::display() const
{
	if (isNull())
		return QString();

	QString result = data()->display().isEmpty()
			? data()->nickName().isEmpty()
					? data()->firstName()
					: data()->nickName()
			: data()->display();

	if (result.isEmpty())
	{
		if (!data()->contacts().isEmpty())
		{
			Contact contact = data()->contacts().at(0);
			if (contact)
			{
				result = contact.id();
				Account account = contact.contactAccount();
				if (!account.accountIdentity().name().isEmpty())
					result += QString(" (%1)").arg(account.accountIdentity().name());
			}
		}
	}

	return result;
}

Buddy Buddy::dummy()
{
	Buddy example = Buddy::create();

	example.setFirstName("Mark");
	example.setLastName("Smith");
	example.setNickName("Jimbo");
	example.setDisplay("Jimbo");
	example.setMobile("+48123456789");
	example.setEmail("jimbo@mail.server.net");
	example.setHomePhone("+481234567890");

	Identity identity = Identity::create();
	identity.setName(identity.data()->tr("Example identity"));

	Account account = Account::create();
	account.setAccountIdentity(identity);

	Contact contact = Contact::create();
	contact.setContactAccount(account);
	contact.setOwnerBuddy(example);
	contact.setId("999999");
	contact.setCurrentStatus(Status(StatusTypeAway, example.data()->tr("Example description")));
	contact.setAddress(QHostAddress(2130706433));
	contact.setPort(80);

	// this is just an example contact, do not add avatar to list
	Avatar avatar = AvatarManager::instance()->byContact(contact, ActionCreate);

	avatar.setLastUpdated(QDateTime::currentDateTime());
	avatar.setFilePath(KaduIcon("kadu_icons/buddy0", "96x96").fullPath());

	example.addContact(contact);
	example.setAnonymous(false);

	return example;
}

KaduSharedBase_PropertyDefCRW(Buddy, Avatar, buddyAvatar, BuddyAvatar, Avatar::null)
KaduSharedBase_PropertyWriteDef(Buddy, const QString &, display, Display)
KaduSharedBase_PropertyDefCRW(Buddy, QString, firstName, FirstName, QString())
KaduSharedBase_PropertyDefCRW(Buddy, QString, lastName, LastName, QString())
KaduSharedBase_PropertyDefCRW(Buddy, QString, familyName, FamilyName, QString())
KaduSharedBase_PropertyDefCRW(Buddy, QString, city, City, QString())
KaduSharedBase_PropertyDefCRW(Buddy, QString, familyCity, FamilyCity, QString())
KaduSharedBase_PropertyDefCRW(Buddy, QString, nickName, NickName, QString())
KaduSharedBase_PropertyDefCRW(Buddy, QString, homePhone, HomePhone, QString())
KaduSharedBase_PropertyDefCRW(Buddy, QString, mobile, Mobile, QString())
KaduSharedBase_PropertyDefCRW(Buddy, QString, email, Email, QString())
KaduSharedBase_PropertyDefCRW(Buddy, QString, website, Website, QString())
KaduSharedBase_PropertyDef(Buddy, unsigned short, birthYear, BirthYear, 0)
KaduSharedBase_PropertyDef(Buddy, BuddyGender, gender, Gender, GenderUnknown)
KaduSharedBase_PropertyDefCRW(Buddy, QList<Group>, groups, Groups, QList<Group>())
KaduSharedBase_PropertyDef(Buddy, bool, preferHigherStatuses, PreferHigherStatuses, true)
KaduSharedBase_PropertyBoolDef(Buddy, Anonymous, true)
KaduSharedBase_PropertyBoolDef(Buddy, Blocked, false)
KaduSharedBase_PropertyBoolDef(Buddy, OfflineTo, false)
KaduSharedBase_PropertyReadDef(Buddy, quint16, unreadMessagesCount, UnreadMessagesCount, 0)
