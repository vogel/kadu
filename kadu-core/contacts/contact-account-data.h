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

#include "protocols/status.h"

class Account;
class XmlConfigFile;

class ContactAccountData : public QObject
{
	Q_OBJECT

	Account *ContactAccount;
	QString Id;

	Status CurrentStatus;

	QString ProtocolVersion;

	QHostAddress Address;
	unsigned int Port;
	QString DnsName;

	bool Blocked;
	bool OfflineTo;

public:
	ContactAccountData();
	ContactAccountData(Account *account, const QString &id);

	virtual bool validateId() = 0;
	virtual void loadConfiguration(XmlConfigFile *configurationStorage, QDomElement parent);
	virtual void storeConfiguration(XmlConfigFile *configurationStorage, QDomElement parent);

	Account * account() { return ContactAccount; }
	QString id() { return Id; }

	bool isValid();

	bool hasFeature() { return false; }

	QString protocolVersion() { return ProtocolVersion; }
	QHostAddress ip() { return Address; }
	unsigned int port() { return Port; }
	void setProtocolVersion(const QString &protocolVersion) { ProtocolVersion = protocolVersion; }
	void setAddressAndPort(QHostAddress address, unsigned int port);

	QString dnsName() { return DnsName; }
	void refreshDNSName();

	Status status() { return CurrentStatus; }
	void setStatus(Status status) { CurrentStatus = status; }

	// properties

	bool isBlocked() { return Blocked; }
	void setBlocked(bool blocked) { Blocked = blocked; }

	bool isOfflineTo() { return OfflineTo; }
	bool setOfflineTo(bool offlineTo) { OfflineTo = offlineTo; }

public slots:
	void setDnsName(const QString &ident, const QString &dnsName) { DnsName = dnsName; }

};

#endif
