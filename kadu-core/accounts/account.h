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

#include "status.h"

class AccountData;
class Contact;
class Protocol;
class XmlConfigFile;

class Account : public QObject
{
	Q_OBJECT

	QUuid Uuid;
	Protocol *ProtocolHandler;
	AccountData *Data;

public:
	Account(const QUuid &uuid);
	Account(const QUuid &uuid, Protocol *protocol, AccountData *data);
	virtual ~Account();

	bool loadConfiguration(XmlConfigFile *configurationStorage, QDomElement parent);
	void storeConfiguration(XmlConfigFile *configurationStorage, QDomElement parent);

	QUuid uuid() { return Uuid; }
	Protocol * protocol() { return ProtocolHandler; }
	QString name();
	AccountData * data() { return Data; }

	UserStatus currentStatus();

	Contact getContactById(const QString &id);

};

#endif // ACCOUNT_H
