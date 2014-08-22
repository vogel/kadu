/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "contacts/contact.h"
#include "message/message.h"
#include "storage/manager.h"
#include "exports.h"

class Account;

class KADUAPI ContactManager : public QObject, public Manager<Contact>
{
	Q_OBJECT
	Q_DISABLE_COPY(ContactManager)

	friend class Core;

	static ContactManager * Instance;

	QList<Contact> DirtyContacts;

	ContactManager();
	virtual ~ContactManager();

	void init();

private slots:
	void removeDuplicateContacts();

	void contactDataUpdated();
	void dirtinessChanged();

	void unreadMessageAdded(const Message &message);
	void unreadMessageRemoved(const Message &message);

protected:
	virtual void loaded();

	virtual void itemAboutToBeRegistered(Contact item);
	virtual void itemRegistered(Contact item);
	virtual void itemAboutToBeUnregisterd(Contact item);
	virtual void itemUnregistered(Contact item);

public:
	enum AnonymousInclusion
	{
		IncludeAnonymous,
		ExcludeAnonymous
	};

	static ContactManager * instance();

	virtual QString storageNodeName() { return QLatin1String("Contacts"); }
	virtual QString storageNodeItemName() { return QLatin1String("Contact"); }

	Contact byId(Account account, const QString &id, NotFoundAction action);
	QVector<Contact> contacts(Account account, AnonymousInclusion inclusion = IncludeAnonymous);

	QVector<Contact> dirtyContacts(Account account);

signals:
	void contactAboutToBeAdded(Contact contact);
	void contactAdded(Contact contact);
	void contactAboutToBeRemoved(Contact contact);
	void contactRemoved(Contact contact);

	void dirtyContactAdded(Contact contact);

	void contactUpdated(const Contact &contact);

};

#endif // CONTACT_MANAGER_H
