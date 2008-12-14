/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ACCOUNT_DATA
#define ACCOUNT_DATA

#include <QtCore/QObject>
#include <QtXml/QDomElement>

class XmlConfigFile;

class AccountData : public QObject
{
	Q_OBJECT

	QString Name;
	QString Id;
	QString Password;

public:
	AccountData();
	AccountData(const QString &id, const QString &password);
	virtual ~AccountData();

	QString name() { return Name; }
	QString password() { return Password; }
	QString id() { return Id; }

	void setName(const QString &name) { Name = name; }
	void setPassword(const QString &password) { Password = password; }
	virtual bool setId(const QString &id);

	virtual bool validateId(const QString &id) = 0;
	virtual bool loadConfiguration(XmlConfigFile *configurationStorage, QDomElement parent) = 0;
	virtual void storeConfiguration(XmlConfigFile *configurationStorage, QDomElement parent) = 0;

};

#endif // ACCOUNT_DATA
