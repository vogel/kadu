/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ACCOUNT_DATA_H
#define ACCOUNT_DATA_H

#include <QtCore/QObject>
#include <QtCore/QSharedData>
#include <QtCore/QUuid>

#include "configuration/uuid-storable-object.h"

#define Property(type, name, capitalized_name) \
	type name() { ensureLoaded(); return capitalized_name; } \
	void set##capitalized_name(const type &name) { ensureLoaded(); capitalized_name = name; dataUpdated(); }

class AccountDetails;
class Protocol;

class KADUAPI AccountData : public QObject, public QSharedData, public UuidStorableObject
{
	Q_OBJECT
	Q_DISABLE_COPY(AccountData)

public:
	enum AccountType
	{
		TypeNull = 0,
		TypeNormal = 1
	};

private:
	QUuid Uuid;
	AccountType Type;
	
	int BlockUpdatedSignalCount;
	bool Updated;

	QString ProtocolName;
	Protocol *ProtocolHandler;
	AccountDetails *Details;
	
	QString Name;
	QString Id;

	bool RememberPassword;
	bool HasPassword;
	QString Password;

	bool ConnectAtStart;

	bool UseProxy;
	QHostAddress ProxyHost;
	short int ProxyPort;
	bool ProxyRequiresAuthentication;
	QString ProxyUser;
	QString ProxyPassword;

	void dataUpdated();
	void emitUpdated();

public:
	static AccountData * loadFromStorage(StoragePoint *accountStoragePoint);

	explicit AccountData(AccountType type, QUuid uuid = QUuid());
	virtual ~AccountData();

	virtual void load();
	virtual void store();

	virtual QUuid uuid() const { return Uuid; }
	void setUuid(const QUuid uuid) { Uuid = uuid; }

	Property(QString, name, Name)
	Property(QString, id, Id)
	Property(bool, rememberPassword, RememberPassword)
	Property(bool, hasPassword, HasPassword)
	Property(QString, password, Password)
	Property(bool, connectAtStart, ConnectAtStart)
	Property(bool, useProxy, UseProxy)
	Property(QHostAddress, proxyHost, ProxyHost)
	Property(short int, proxyPort, ProxyPort)
	Property(bool, proxyRequiresAuthentication, ProxyRequiresAuthentication)
	Property(QString, proxyUser, ProxyUser)
	Property(QString, proxyPassword, ProxyPassword)

signals:
	void updated();

};

#endif // ACCOUNT_DATA_H
