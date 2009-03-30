/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QtCore/QObject>
#include <QtCore/QUuid>
#include <QtXml/QDomElement>

#include "contacts/contact.h"
#include "contacts/contacts-aware-object.h"
#include "protocols/status.h"

class QPixmap;

class AccountData;
class Protocol;
class Status;
class XmlConfigFile;

class KADUAPI Account : public QObject, public UuidStorableObject, public ContactsAwareObject
{
	Q_OBJECT

	QUuid Uuid;
	Protocol *ProtocolHandler;

	QString Name;
	QString Id;
	QString Password;

protected:
	virtual void contactAdded(Contact contact);
	virtual void contactRemoved(Contact contact);

public:
	Account(const QUuid &uuid = QUuid());
	virtual ~Account();

	virtual void loadConfiguration();
	virtual void storeConfiguration();

	virtual QUuid uuid() { return Uuid; }
	void setProtocol(Protocol *protocolHandler);
	Protocol * protocol() { return ProtocolHandler; }

	void setName(const QString &name) { Name = name; }
	void setPassword(const QString &password) { Password = password; }
	virtual bool setId(const QString &id);

	QString name() { return Name; }
	QString password() { return Password; }
	QString id() { return Id; }

	Status currentStatus();

	Contact getContactById(const QString &id);
	Contact createAnonymous(const QString &id);

	QPixmap statusPixmap(Status status);

signals:
	void contactStatusChanged(Account *account, Contact contact, Status oldStatus);

};

#endif // ACCOUNT_H
