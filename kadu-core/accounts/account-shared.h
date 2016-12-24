/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2010, 2011, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2012, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "network/proxy/network-proxy.h"
#include "status/status.h"
#include "storage/shared.h"

#include <QtCore/QPointer>
#include <QtNetwork/QHostAddress>
#include <injeqt/injeqt.h>

class AccountManager;
class AccountStatusContainer;
class Account;
class Configuration;
class ContactManager;
class Contact;
class FileTransferService;
class IdentityManager;
class Identity;
class InjectedFactory;
class NetworkProxyManager;
class ProtocolFactory;
class ProtocolsManager;
class Protocol;
class RosterTask;
class StatusContainer;
class StatusSetter;

class KADUAPI AccountShared : public Shared
{
	Q_OBJECT

	friend class AccountStatusContainer;

	QPointer<AccountManager> m_accountManager;
	QPointer<Configuration> m_configuration;
	QPointer<ContactManager> m_contactManager;
	QPointer<IdentityManager> m_identityManager;
	QPointer<InjectedFactory> m_injectedFactory;
	QPointer<NetworkProxyManager> m_networkProxyManager;
	QPointer<ProtocolsManager> m_protocolsManager;
	QPointer<StatusSetter> m_statusSetter;

	QString ProtocolName;
	Protocol *ProtocolHandler;
	AccountStatusContainer *MyStatusContainer;

	Identity *AccountIdentity;
	Contact *AccountContact;

	QString Id;

	bool RememberPassword;
	bool HasPassword;
	QString Password;

	bool UseDefaultProxy;
	NetworkProxy Proxy;

	bool PrivateStatus;

	void setDisconnectStatus();
	void useProtocolFactory(ProtocolFactory *factory);

	void doSetAccountIdentity(const Identity &accountIdentity);
	void doSetId(const QString &id);

	void importNetworkProxy();

	QVector<RosterTask> loadRosterTasks();
	void storeRosterTasks(const QVector<RosterTask> &tasks);

private slots:
	INJEQT_SET void setAccountManager(AccountManager *accountManager);
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_SET void setContactManager(ContactManager *contactManager);
	INJEQT_SET void setIdentityManager(IdentityManager *identityManager);
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
	INJEQT_SET void setNetworkProxyManager(NetworkProxyManager *networkProxyManager);
	INJEQT_SET void setProtocolsManager(ProtocolsManager *protocolsManager);
	INJEQT_SET void setStatusSetter(StatusSetter *statusSetter);
	INJEQT_INIT void init();

	void protocolRegistered(ProtocolFactory *protocolHandler);
	void protocolUnregistered(ProtocolFactory *protocolHandler);

protected:
	virtual void load();
	virtual void store();
	virtual bool shouldStore();

	// TODO: 0.11, fix this
	// hack, changing details does not trigger this
	friend class GaduEditAccountWidget;
	void forceEmitUpdated();

public:
	explicit AccountShared(const QString &protocolName = QString(), QObject *parent = nullptr);
	virtual ~AccountShared();

	virtual StorableObject * storageParent();
	virtual QString storageNodeName();

	virtual void aboutToBeRemoved();

	Contact accountContact();

	StatusContainer * statusContainer();

	KaduShared_PropertyDeclCRW(Identity, accountIdentity, AccountIdentity)

	KaduShared_PropertyRead(const QString &, protocolName, ProtocolName)

	void setId(const QString &id);
	KaduShared_PropertyRead(const QString &, id, Id)

	void setPrivateStatus(bool isPrivate);
	KaduShared_PropertyRead(bool, privateStatus, PrivateStatus)

	KaduShared_PropertyRead(Protocol *, protocolHandler, ProtocolHandler)
	KaduShared_Property(bool, rememberPassword, RememberPassword)
	KaduShared_Property(bool, hasPassword, HasPassword)
	KaduShared_Property(const QString &, password, Password)
	KaduShared_Property(bool, useDefaultProxy, UseDefaultProxy)
	KaduShared_Property(const NetworkProxy &, proxy, Proxy)

signals:
	void buddyStatusChanged(const Contact &contact, const Status &oldStatus);

	void connecting();
	void connected();
	void disconnected();

	void remoteStatusChangeRequest(Account account, Status requestedStatus);

	void updated();
	void protocolHandlerChanged(Account account);

};

KADUAPI Protocol * protocol(AccountShared *account);
