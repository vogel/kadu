/*
 * %kadu copyright begin%
 * Copyright 2008, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
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

#ifndef GADU_USERLIST_HANDLER_H
#define GADU_USERLIST_HANDLER_H

#include <QtCore/QObject>

#include <protocols/protocol.h>

class Contact;
class GaduContactDetails;
class GaduProtocol;

class GaduContactListHandler : public QObject
{
	Q_OBJECT

	GaduProtocol *Protocol;
	bool AlreadySent;

private slots:
	void buddySubscriptionChanged(Buddy &buddy);
	void contactAttached(Contact contact, bool reattached);
	void contactDetached(Contact contact, Buddy previousBuddy, bool reattaching);
	void contactIdChanged(Contact contact, const QString &oldId);

public:
	static int notifyTypeFromContact(const Contact &contact);

	explicit GaduContactListHandler(GaduProtocol *protocol);
	virtual ~GaduContactListHandler();

	void setUpContactList(const QVector<Contact> &contacts);
	void reset();

	void updateContactEntry(Contact contact);

};

#endif // GADU_USERLIST_HANDLER_H
