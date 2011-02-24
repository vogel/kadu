/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
class GaduProtocol;

class GaduContactListHandler : public QObject
{
	Q_OBJECT

	GaduProtocol *Protocol;

public:
	static int notifyTypeFromContact(const Contact &contact);

	explicit GaduContactListHandler(GaduProtocol *protocol, QObject *parent = 0);
	virtual ~GaduContactListHandler();

	void setUpContactList(const QList<Contact> &contacts);

	void updateContactEntry(Contact contact);

	void addContactEntry(UinType uin, int type);
	void addContactEntry(Contact contact);

	void removeContactEntry(UinType uin);
	void removeContactEntry(Contact contact);

};

#endif // GADU_USERLIST_HANDLER_H
