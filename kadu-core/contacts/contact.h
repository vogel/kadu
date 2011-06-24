/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
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

#ifndef CONTACT_H
#define CONTACT_H

#include <QtNetwork/QHostAddress>
#include <QtXml/QDomElement>

#include "buddies/buddy.h"
#include "status/status.h"
#include "storage/shared-base.h"

#include "exports.h"

class Avatar;
class ContactDetails;
class ContactShared;
class XmlConfigFile;

class KADUAPI Contact : public SharedBase<ContactShared>
{
	KaduSharedBaseClass(Contact)

public:
	static Contact create();
	static Contact loadStubFromStorage(const QSharedPointer<StoragePoint> &storage);
	static Contact loadFromStorage(const QSharedPointer<StoragePoint> &storage);
	static Contact null;

	static Contact contactWithHigherStatus(const Contact &c1, const Contact &c2);

	Contact();
	Contact(ContactShared *data);
	Contact(QObject *data);
	Contact(const Contact &copy);
	virtual ~Contact();

	KaduSharedBase_Property(ContactDetails *, details, Details)
	KaduSharedBase_PropertyRead(QUuid, uuid, Uuid)
	KaduSharedBase_PropertyRead(QSharedPointer<StoragePoint>, storage, Storage)
	KaduSharedBase_PropertyCRW(Account, contactAccount, ContactAccount)
	KaduSharedBase_PropertyCRW(Avatar, contactAvatar, ContactAvatar)
	KaduSharedBase_PropertyCRW(Buddy, ownerBuddy, OwnerBuddy)
	KaduSharedBase_PropertyCRW(QString, id, Id)
	KaduSharedBase_Property(int, priority, Priority)
	KaduSharedBase_PropertyCRW(Status, currentStatus, CurrentStatus)
	KaduSharedBase_PropertyBool(Blocking)
	KaduSharedBase_PropertyBool(Dirty)
	KaduSharedBase_PropertyCRW(QString, protocolVersion, ProtocolVersion)
	KaduSharedBase_PropertyCRW(QHostAddress, address, Address)
	KaduSharedBase_Property(unsigned int, port, Port)
	KaduSharedBase_PropertyCRW(QString, dnsName, DnsName)
	KaduSharedBase_Property(short int, maximumImageSize, MaximumImageSize)

};

Q_DECLARE_METATYPE(Contact)

#endif // CONTACT_H
