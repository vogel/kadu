/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ACCOUNT_SHARED_H
#define ACCOUNT_SHARED_H

#include <QtCore/QObject>
#include <QtCore/QSharedData>
#include <QtCore/QUuid>

#include "shared/shared.h"
#include "status/base-status-container.h"

class AccountDetails;
class Buddy;
class Protocol;
class ProtocolFactory;
class StatusType;

class KADUAPI AccountShared : public BaseStatusContainer, public Shared
{
	Q_OBJECT
	Q_DISABLE_COPY(AccountShared)

public:
	enum AccountType
	{
		TypeNull = 0,
		TypeNormal = 1
	};

private:
	AccountType Type;

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

protected:
	void emitUpdated();

public:
	static AccountShared * loadFromStorage(StoragePoint *storagePoint);

	explicit AccountShared(AccountType type, QUuid uuid = QUuid());
	virtual ~AccountShared();

	virtual void load();
	virtual void store();

	//account type
	bool isNull() const { return TypeNull == Type; }

	void loadProtocol(ProtocolFactory *protocolFactory);
	void unloadProtocol();

	KaduShared_Property(QString, protocolName, ProtocolName)
	KaduShared_Property(Protocol *, protocolHandler, ProtocolHandler)
	KaduShared_Property(AccountDetails *, details, Details)
	KaduShared_Property(QString, name, Name)
	KaduShared_Property(QString, id, Id)
	KaduShared_Property(bool, rememberPassword, RememberPassword)
	KaduShared_Property(bool, hasPassword, HasPassword)
	KaduShared_Property(QString, password, Password)
	KaduShared_Property(bool, connectAtStart, ConnectAtStart)
	KaduShared_Property(bool, useProxy, UseProxy)
	KaduShared_Property(QHostAddress, proxyHost, ProxyHost)
	KaduShared_Property(short int, proxyPort, ProxyPort)
	KaduShared_Property(bool, proxyRequiresAuthentication, ProxyRequiresAuthentication)
	KaduShared_Property(QString, proxyUser, ProxyUser)
	KaduShared_Property(QString, proxyPassword, ProxyPassword)

	// StatusContainer implementation

	virtual QString statusContainerName();

	virtual void setStatus(Status newStatus);
	virtual const Status & status();
	virtual int maxDescriptionLength();

	virtual QString statusName();
	virtual QPixmap statusPixmap();
	virtual QPixmap statusPixmap(const QString &statusType);

	virtual QList<StatusType *> supportedStatusTypes();

	QPixmap statusPixmap(Status status);

	virtual void setPrivateStatus(bool isPrivate);

signals:
	void buddyStatusChanged(Account account, Buddy buddy, Status oldStatus);

	void updated();

};

#include "buddies/buddy.h" // for MOC

#endif // ACCOUNT_SHARED_H
