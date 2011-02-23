/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include <QtDBus/QDBusArgument>
#include <QtDBus/QDBusMetaType>

#include "accounts/account-manager.h"
#include "buddies/buddy-manager.h"
#include "contacts/contact-manager.h"
#include "debug.h"

#include "kadudbus.h"
#include "kadurootobject.h"

extern "C" KADU_EXPORT int dbus_init(bool)
{
	dbus = new KaduDBus();
	return 0;
}

extern "C" KADU_EXPORT void dbus_close()
{
	delete dbus;
	dbus = NULL;
}

QDBusArgument &operator<<(QDBusArgument &argument, const Contact &contact)
{
	argument.beginStructure();
	argument << contact.uuid();
	argument.endStructure();
	return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, Contact &contact)
{
	argument.beginStructure();
	contact = ContactManager::instance()->byUuid(QUuid(argument.asVariant().toString()));
	argument.endStructure();
	return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const QList<Contact> &contacts)
{
	argument.beginStructure();
	foreach (const Contact contact, contacts)
		argument << contact.uuid();
	argument.endStructure();
	return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, QList<Contact> &contacts)
{
	argument.beginStructure();
	// TODO
	contacts.append(ContactManager::instance()->byUuid(QUuid(argument.asVariant().toString())));
	argument.endStructure();
	return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const Account &account)
{
	argument.beginStructure();
	argument << account.uuid();
	argument.endStructure();
	return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, Account &account)
{
	argument.beginStructure();
	account = AccountManager::instance()->byUuid(QUuid(argument.asVariant().toString()));
	argument.endStructure();
	return argument;
}

KaduDBus::KaduDBus() : QObject(), connection(QDBusConnection::sessionBus())
{
	qDBusRegisterMetaType<Contact>();
	qDBusRegisterMetaType<Account>();

	KaduRootObject *root = new KaduRootObject(this);
	//connection = QDBusConnection::sessionBus();
	qDebug() << "Root Object = " << connection.registerObject("/", root, QDBusConnection::ExportAllContents);
	//connection.registerObject("/Buddies", BuddyManager::instance(), QDBusConnection::ExportAllContents);
	qDebug() << "Contacts Object = " << connection.registerObject("/Contacts", ContactManager::instance(), QDBusConnection::ExportAllContents | QDBusConnection::ExportChildObjects);
	connection.registerService("im.kadu");
}

KaduDBus::~KaduDBus()
{
	connection.unregisterService("im.kadu");
}

KaduDBus* dbus;
