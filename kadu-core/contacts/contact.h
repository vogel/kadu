/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONTACT_H
#define CONTACT_H

#include <QtNetwork/QHostAddress>
#include <QtXml/QDomElement>

#include "accounts/account.h"
#include "buddies/buddy.h"
#include "shared/shared-base.h"
#include "status/status.h"

#include "exports.h"

class Avatar;
class ContactDetails;
class ContactShared;
class XmlConfigFile;

class KADUAPI Contact : public QObject, public SharedBase<ContactShared>
{
	Q_OBJECT

protected:
	virtual void connectDataSignals();
	virtual void disconnectDataSignals();

public:
	static Contact loadFromStorage(StoragePoint *storage);
	static Contact null;
	
	explicit Contact(bool null = false);
	explicit Contact(ContactShared *data);
	Contact(const Contact &copy);
	virtual ~Contact();

	Contact & operator = (const Contact &copy);

	void loadDetails();
	void unloadDetails();

	virtual bool validateId();
	bool isValid();

	KaduSharedBase_Property(ContactDetails *, details, Details, 0)
	KaduSharedBase_PropertyRead(QUuid, uuid, Uuid, QUuid())
	KaduSharedBase_PropertyRead(StoragePoint *, storage, Storage, 0)
	KaduSharedBase_Property(Account, contactAccount, ContactAccount, Account::null)
	Avatar & contactAvatar() const;
	KaduSharedBase_Property(Buddy, ownerBuddy, OwnerBuddy, Buddy::null)
	KaduSharedBase_Property(QString, id, Id, QString::null)
	KaduSharedBase_Property(Status, currentStatus, CurrentStatus, Status::null)
	KaduSharedBase_Property(QString, protocolVersion, ProtocolVersion, QString::null)
	KaduSharedBase_Property(QHostAddress, address, Address, QHostAddress())
	KaduSharedBase_Property(unsigned int, port, Port, 0)
	KaduSharedBase_Property(QString, dnsName, DnsName, QString::null)

signals:
	void idChanged(const QString &id);

};

Q_DECLARE_METATYPE(Contact)

#endif // CONTACT_H
