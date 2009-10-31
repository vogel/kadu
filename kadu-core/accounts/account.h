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

#include "accounts/account-data.h"
#include "contacts/contact.h"
#include "contacts/contacts-aware-object.h"
#include "status/base-status-container.h"
#include "status/status.h"

#undef PropertyRead
#define PropertyRead(type, name, capitalized_name, default) \
	type name() const\
	{\
		return !Data\
			? default\
			: Data->name();\
	}

#undef PropertyWrite
#define PropertyWrite(type, name, capitalized_name, default) \
	void set##capitalized_name(type name) const\
	{\
		if (Data)\
			Data->set##capitalized_name(name);\
	}

#undef Property
#define Property(type, name, capitalized_name, default) \
	PropertyRead(type, name, capitalized_name, default) \
	PropertyWrite(type, name, capitalized_name, default)

class QPixmap;

class AccountDetails;
class Protocol;
class ProtocolFactory;
class Status;
class XmlConfigFile;

class KADUAPI Account : public QObject
{
	Q_OBJECT

	QExplicitlySharedDataPointer<AccountData> Data;

	void connectDataSignals();
	void disconnectDataSignals();

public:
	static Account loadFromStorage(StoragePoint *storage);
	static Account null;

	explicit Account(AccountData::AccountType type = AccountData::TypeNormal);
	explicit Account(AccountData *data);
	Account(const Account &copy);
	virtual ~Account();

	AccountData * data() const { return Data.data(); }

	bool isNull() const;

	Account & operator = (const Account &copy);
	bool operator == (const Account &compare) const;
	bool operator != (const Account &compare) const;
	int operator < (const Account &compare) const;

	void store();
	void removeFromStorage();

	QUuid uuid() const;
	StoragePoint * storage() const;

	void loadProtocol(ProtocolFactory *protocolFactory);
	void unloadProtocol();

	void importProxySettings();

	Contact getContactById(const QString &id);
	Contact createAnonymous(const QString &id);

	StatusContainer * statusContainer() { return Data.data(); }

	Property(QString, protocolName, ProtocolName, QString::null)
	Property(Protocol *, protocolHandler, ProtocolHandler, 0)
	Property(AccountDetails *, details, Details, 0)
	Property(QString, name, Name, QString::null)
	Property(QString, id, Id, QString::null)
	Property(bool, rememberPassword, RememberPassword, true)
	Property(bool, hasPassword, HasPassword, false)
	Property(QString, password, Password, QString::null)
	Property(bool, connectAtStart, ConnectAtStart, true)
	Property(bool, useProxy, UseProxy, false)
	Property(QHostAddress, proxyHost, ProxyHost, QHostAddress())
	Property(short int, proxyPort, ProxyPort, 0)
	Property(bool, proxyRequiresAuthentication, ProxyRequiresAuthentication, false)
	Property(QString, proxyUser, ProxyUser, QString::null)
	Property(QString, proxyPassword, ProxyPassword, QString::null)

signals:
	void contactStatusChanged(Account account, Contact contact, Status oldStatus);

};

Q_DECLARE_METATYPE(Account)

uint qHash(const Account &account);

#endif // ACCOUNT_H
