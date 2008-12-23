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

class ContactAccountData
{
	Account *ContactAccount;
	QString Id;

	Status CurrentStatus;

	QString ProtocolVersion;

	QHostAddress Address;
	unsigned int Port;

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

	void setProtocolVersion(const QString &protocolVersion) { ProtocolVersion = protocolVersion; }
	void setAddressAndPort(QHostAddress address, int port);

	Status status() { return CurrentStatus; }
	void setStatus(Status status) { CurrentStatus = status; }

};

#endif
