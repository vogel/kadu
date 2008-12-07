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
#include <QtXml/QDomElement>

#include "status.h"

class AccountData;
class Protocol;
class XmlConfigFile;

class Account : public QObject
{
	Q_OBJECT

	QString Name;
	Protocol *ProtocolHandler;
	AccountData *Data;

public:
	Account(const QString &name);
	Account(const QString &name, Protocol *protocol, AccountData *data);
	virtual ~Account();

	bool loadConfiguration(XmlConfigFile *configurationStorage, QDomElement parent);
	void storeConfiguration(XmlConfigFile *configurationStorage, QDomElement parent);

	QString name() { return Name; }
	Protocol * protocol() { return ProtocolHandler; }

	UserStatus currentStatus();

};

#endif // ACCOUNT_H
