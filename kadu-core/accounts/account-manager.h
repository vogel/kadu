/*
 * %kadu copyright begin%
 * Copyright 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
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

#ifndef ACCOUNT_MANAGER_H
#define ACCOUNT_MANAGER_H

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QUuid>

#include "accounts/account.h"
#include "protocols/protocol-factory.h"
#include "storage/manager.h"
#include "exports.h"

class AccountShared;
class Status;
class XmlConfigFile;

class KADUAPI AccountManager : public QObject, public Manager<Account>
{
	Q_OBJECT
	Q_DISABLE_COPY(AccountManager)

	static AccountManager *Instance;

	AccountManager();
	virtual ~AccountManager();

	friend class AccountShared;
	void detailsLoaded(Account account);
	void detailsUnloaded(Account account);

	virtual void loaded();

private slots:
	void accountDataUpdated();
	void connectionError(Account account, const QString &server, const QString &message);
    void invalidPassword(Account account);

protected:
	virtual void itemAdded(Account item);
	virtual void itemAboutToBeRemoved(Account item);
	virtual void itemRemoved(Account item);

	virtual void itemAboutToBeRegistered(Account item);
	virtual void itemRegistered(Account item);
	virtual void itemAboutToBeUnregisterd(Account item);
	virtual void itemUnregistered(Account item);

public:
	static AccountManager * instance();

	virtual QString storageNodeName() { return QLatin1String("Accounts"); }
	virtual QString storageNodeItemName() { return QLatin1String("Account"); }

	Account defaultAccount();

	const QList<Account> byIdentity(Identity identity);
	Account byId(const QString &protocolName, const QString &id);
	const QList<Account> byProtocolName(const QString &name);

	Status status();

	void removeAccountAndBuddies(Account account);

signals:
	void accountAboutToBeRegistered(Account);
	void accountRegistered(Account);
	void accountAboutToBeUnregistered(Account);
	void accountUnregistered(Account);

	void accountAboutToBeRemoved(Account);
	void accountRemoved(Account);

	void accountUpdated(Account);

};

#endif // ACCOUNT_MANAGER_H
