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

	bool UseProxy;
	QHostAddress ProxyHost;
	short int ProxyPort;
	bool ProxyReqAuthentication;
	QString ProxyUser;
	QString ProxyPassword;

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

	bool useProxy() { return UseProxy; }
	QHostAddress proxyHost() { return ProxyHost; }
	short int proxyPort() { return ProxyPort; }
	bool proxyReqAuthentication() { return ProxyReqAuthentication; }
	QString proxyUser() { return ProxyUser; }
	QString proxyPassword() { return ProxyPassword; }

	void setUseProxy(bool useProxy) { UseProxy = useProxy; }
	void setProxyHost(QHostAddress proxyHost) { ProxyHost = proxyHost; }
	void setProxyPort(short int proxyPort) { ProxyPort = proxyPort; }
	void setProxyReqAuthentication(bool proxyReqAuth) { ProxyReqAuthentication = proxyReqAuth; }
	void setProxyUser(QString proxyUser) { ProxyUser = proxyUser; }
	void setProxyPassword(QString proxyPassword) { ProxyPassword = proxyPassword; }

	void importProxySettings();

	Contact getContactById(const QString &id);
	Contact createAnonymous(const QString &id);

	// StatusContainer implementation

	virtual QString statusContainerName();

	virtual void setStatus(Status newStatus);
	virtual Status status();
	virtual int maxDescriptionLength();

	virtual QString statusName();
	virtual QPixmap statusPixmap();
	virtual QPixmap statusPixmap(const QString &statusType);

	virtual QList<StatusType *> supportedStatusTypes();

	QPixmap statusPixmap(Status status);

	virtual void setDefaultStatus(const QString &startupStatus, bool offlineToInvisible,
				      const QString &startupDescription, bool StartupLastDescription);
	virtual void disconnectAndStoreLastStatus(bool disconnectWithCurrentDescription,
						  const QString &disconnectDescription);

signals:
	void contactStatusChanged(Account *account, Contact contact, Status oldStatus);

};

Q_DECLARE_METATYPE(Account *)

#endif // ACCOUNT_H
