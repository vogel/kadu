/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONTACT_ACCOUNT_DATA
#define CONTACT_ACCOUNT_DATA

#include <QtNetwork/QHostAddress>
#include <QtXml/QDomElement>

#include "configuration/storable-object.h"
#include "protocols/status.h"

#include "contact.h"

class Account;
class XmlConfigFile;

class ContactAccountData : public QObject, public StorableObject
{
	Q_OBJECT

	Account *ContactAccount;
	Contact OwnerContact;
	QString Id;

	Status CurrentStatus;

	QString ProtocolVersion;

	QHostAddress Address;
	unsigned int Port;
	QString DnsName;

	bool Blocked;
	bool OfflineTo;

protected:
	virtual StoragePoint * createStoragePoint();

public:
	ContactAccountData(Contact contact, Account *account, const QString &id = QString::null);

	virtual bool validateId() = 0;
	virtual void loadConfiguration();
	virtual void storeConfiguration();

	Account * account() { return ContactAccount; }
	QString id() { return Id; }

	bool isValid();

	bool hasFeature() { return false; }

	QString protocolVersion() { return ProtocolVersion; }
	void setProtocolVersion(QString protocolVersion) { ProtocolVersion = protocolVersion; }

	QHostAddress ip() { return Address; }
	void setIp(QHostAddress addres) { Address = addres; }

	unsigned int port() { return Port; }
	void setPort(int port) { Port = port; }

	QString dnsName() { return DnsName; }
	void refreshDNSName();

	Status status() { return CurrentStatus; }
	void setStatus(Status status) { CurrentStatus = status; }

	// properties

	bool isBlocked() { return Blocked; }
	void setBlocked(bool blocked) { Blocked = blocked; }

	bool isOfflineTo() { return OfflineTo; }
	void setOfflineTo(bool offlineTo) { OfflineTo = offlineTo; }

public slots:
	void setDNSName(const QString &ident, const QString &dnsName) { DnsName = dnsName; }

};

#endif
