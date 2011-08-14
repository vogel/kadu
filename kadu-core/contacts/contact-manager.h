/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#ifndef CONTACT_MANAGER_H
#define CONTACT_MANAGER_H

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QUuid>

#include "contacts/contact-shared.h"
#include "storage/manager.h"
#include "exports.h"

class Account;
class Contact;

class KADUAPI ContactManager : public QObject, public Manager<Contact>
{
	Q_OBJECT
	Q_DISABLE_COPY(ContactManager)

	static ContactManager * Instance;

	QList<Contact> DirtyContacts;

	ContactManager();
	virtual ~ContactManager();

	friend class ContactShared;
	void detailsLoaded(Contact item);
	void detailsUnloaded(Contact item);

private slots:
	void removeDuplicateContacts();

	void contactDataUpdated();
	void idChanged(const QString &oldId);
	void dirtinessChanged();

	void aboutToBeAttached(Buddy nearFutureBuddy);
	void attached(bool reattached);
	void aboutToBeDetached(bool reattaching);
	void detached(Buddy previousBuddy);

protected:
	virtual void loaded();

	virtual void itemAboutToBeRegistered(Contact item);
	virtual void itemRegistered(Contact item);
	virtual void itemAboutToBeUnregisterd(Contact item);
	virtual void itemUnregistered(Contact item);

public:
	static ContactManager * instance();

	virtual QString storageNodeName() { return QLatin1String("Contacts"); }
	virtual QString storageNodeItemName() { return QLatin1String("Contact"); }

	Contact byId(Account account, const QString &id, NotFoundAction = ActionCreate);
	QList<Contact> contacts(Account account);

	const QList<Contact> & dirtyContacts();
	QList<Contact> dirtyContacts(Account account);

signals:
	void contactAboutToBeAdded(Contact contact);
	void contactAdded(Contact contact);
	void contactAboutToBeRemoved(Contact contact);
	void contactRemoved(Contact contact);

	void contactAboutToBeDetached(Contact contact, bool reattaching);
	void contactDetached(Contact contact, Buddy previousBuddy);
	void contactAboutToBeAttached(Contact contact, Buddy nearFutureBuddy);
	void contactAttached(Contact contact, bool reattached);

	void contactIdChanged(Contact contact, const QString &oldId);

	void dirtyContactAdded(Contact contact);

	void contactUpdated(Contact &contact);

};

// for MOC
#include "contacts/contact.h"

#endif // CONTACT_MANAGER_H
