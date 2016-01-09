/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2008, 2009, 2010, 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "contacts/contact.h"
#include "message/message.h"
#include "storage/simple-manager.h"
#include "exports.h"

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QUuid>
#include <injeqt/injeqt.h>

class Account;
class UnreadMessageRepository;

class KADUAPI ContactManager : public QObject, public SimpleManager<Contact>
{
	Q_OBJECT

public:
	enum AnonymousInclusion
	{
		IncludeAnonymous,
		ExcludeAnonymous
	};

	Q_INVOKABLE explicit ContactManager(QObject *parent = nullptr);
	virtual ~ContactManager();

	virtual QString storageNodeName() { return QLatin1String("Contacts"); }
	virtual QString storageNodeItemName() { return QLatin1String("Contact"); }

	Contact byId(Account account, const QString &id, NotFoundAction action);
	QVector<Contact> contacts(Account account, AnonymousInclusion inclusion = IncludeAnonymous);

signals:
	void contactAboutToBeAdded(Contact contact);
	void contactAdded(Contact contact);
	void contactAboutToBeRemoved(Contact contact);
	void contactRemoved(Contact contact);

	void contactUpdated(const Contact &contact);

protected:
	virtual void loaded();

	virtual void itemAboutToBeAdded(Contact item) override;
	virtual void itemAdded(Contact item) override;
	virtual void itemAboutToBeRemoved(Contact item) override;
	virtual void itemRemoved(Contact item) override;

private:
	QPointer<UnreadMessageRepository> m_unreadMessageRepository;

private slots:
	INJEQT_SET void setUnreadMessageRepository(UnreadMessageRepository *unreadMessageRepository);
	INJEQT_INIT void init();
	INJEQT_DONE void done();

	void removeDuplicateContacts();

	void contactDataUpdated();

	void unreadMessageAdded(const Message &message);
	void unreadMessageRemoved(const Message &message);

};
