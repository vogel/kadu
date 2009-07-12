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

#include <QtCore/QUuid>
#include <QtXml/QDomElement>

#include "contacts/contact.h"
#include "contacts/contacts-aware-object.h"
#include "status/status.h"
#include "status/status-container.h"

class QPixmap;

class AccountData;
class Protocol;
class Status;
class XmlConfigFile;

class KADUAPI Account : public StatusContainer, public UuidStorableObject, public ContactsAwareObject
{
	Q_OBJECT

	QUuid Uuid;
	Protocol *ProtocolHandler;

	QString Name;
	QString Id;
	bool RememberPassword;
	bool HasPassword;
	QString Password;

	bool ConnectAtStart;

protected:
	virtual void contactAdded(Contact contact);
	virtual void contactRemoved(Contact contact);

public:
	Account(const QUuid &uuid = QUuid());
	virtual ~Account();

	virtual void load();
	virtual void store();

	virtual QUuid uuid() const { return Uuid; }
	void setProtocol(Protocol *protocolHandler);
	Protocol * protocol() { return ProtocolHandler; }

	void setConnectAtStart(bool connectAtStart) { ConnectAtStart = connectAtStart; }
	bool connectAtStart() { return ConnectAtStart; }

	void setName(const QString &name) { Name = name; }
	virtual bool setId(const QString &id);
	void setRememberPassword(bool rememberPassword) { RememberPassword = rememberPassword; }
	void setPassword(const QString &password) { Password = password; HasPassword = !Password.isEmpty(); }

	QString name() { return Name; }
	QString id() { return Id; }
	bool rememberPassword() { return RememberPassword; }
	bool hasPassword() { return HasPassword; }
	QString password() { return Password; }

	Contact getContactById(const QString &id);
	Contact createAnonymous(const QString &id);

	// StatusContainer implementation

	virtual QString statusContainerName();

	virtual void setStatus(Status newStatus);
	virtual Status status();

	virtual QString statusName();
	virtual QPixmap statusPixmap();
	virtual QPixmap statusPixmap(const QString &statusType);

	virtual QList<StatusType *> supportedStatusTypes();

	QPixmap statusPixmap(Status status);

signals:
	void contactStatusChanged(Account *account, Contact contact, Status oldStatus);

};

#endif // ACCOUNT_H
