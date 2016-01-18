/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "misc/memory.h"
#include "status/storable-status-container.h"
#include "storage/shared.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class Account;
class IdentityManager;
class IdentityStatusContainer;
class InjectedFactory;

class KADUAPI IdentityShared : public QObject, public Shared
{
	Q_OBJECT

public:
	static IdentityShared * loadStubFromStorage(const std::shared_ptr<StoragePoint> &accountStoragePoint);
	static IdentityShared * loadFromStorage(const std::shared_ptr<StoragePoint> &accountStoragePoint);

	explicit IdentityShared(const QUuid &uuid = QUuid());
	virtual ~IdentityShared();

	virtual StorableObject * storageParent();
	virtual QString storageNodeName();

	virtual void aboutToBeRemoved();

	QList<Account> accounts();
	void addAccount(const Account &account);
	void removeAccount(const Account &account);
	bool hasAccount(const Account &account);
	bool hasAnyAccountWithDetails();
	bool isEmpty();

	KaduShared_Property(const QString &, name, Name)
	KaduShared_PropertyBool(Permanent)

	StatusContainer * statusContainer() const;

protected:
	virtual void load();
	virtual void store();
	virtual bool shouldStore();

private:
	QPointer<IdentityManager> m_identityManager;
	QPointer<InjectedFactory> m_injectedFactory;

	not_owned_qptr<IdentityStatusContainer> m_identityStatusContainer;

	bool Permanent;
	QString Name;
	QList<Account> Accounts;

private slots:
	INJEQT_SET void setIdentityManager(IdentityManager *identityManager);
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
	INJEQT_INIT void init();

};
