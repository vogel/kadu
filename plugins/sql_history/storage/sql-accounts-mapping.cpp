/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtSql/QSqlQuery>

#include "accounts/account.h"
#include "accounts/account-manager.h"

#include "sql-accounts-mapping.h"

SqlAccountsMapping::SqlAccountsMapping(const QSqlDatabase &database, QObject *parent) :
		QObject(parent), Database(database)
{
	loadMappingsFromDatabase();

	triggerAllAccountsAdded();

	connect(AccountManager::instance(), SIGNAL(accountUpdated(Account)), this, SLOT(accountUpdated(Account)));
}

SqlAccountsMapping::~SqlAccountsMapping()
{
}

void SqlAccountsMapping::accountAdded(Account account)
{
	if (idByAccount(account) > 0)
		return;

	QSqlQuery query(Database);
	query.prepare("INSERT INTO kadu_accounts (protocol, account) VALUES (:protocol, :account)");
	query.bindValue(":protocol", account.protocolName());
	query.bindValue(":account", account.id());
	query.exec();

	addMapping(query.lastInsertId().toInt(), account);
}

void SqlAccountsMapping::accountRemoved(Account account)
{
	if (idByAccount(account) <= 0)
		return;

	QSqlQuery query(Database);
	query.prepare("UPDATE kadu_accounts SET protocol = '', account = '' WHERE id = :id");
	query.bindValue(":id", idByAccount(account));
	query.exec();
}

void SqlAccountsMapping::accountUpdated(const Account &account)
{
	if (idByAccount(account) <= 0)
		return;

	QSqlQuery query(Database);
	query.prepare("UPDATE kadu_accounts SET protocol = :protocol, account = :account WHERE id = :id");
	query.bindValue(":protocol", account.protocolName());
	query.bindValue(":account", account.id());
	query.bindValue(":id", idByAccount(account));
	query.exec();
}

void SqlAccountsMapping::addMapping(int id, const Account &account)
{
	account.addProperty("sql_history:id", id, CustomProperties::NonStorable);
	AccountMapping.insert(id, account);
}

void SqlAccountsMapping::loadMappingsFromDatabase()
{
	QSqlQuery query(Database);
	query.prepare("SELECT id, protocol, account FROM kadu_accounts");

	query.setForwardOnly(true);
	query.exec();

	while (query.next())
	{
		int id = query.value(0).toInt();
		QString protocol = query.value(1).toString();
		QString accountId = query.value(2).toString();

		if (id <= 0 || protocol.isEmpty() || accountId.isEmpty())
			continue;

		Account account = AccountManager::instance()->byId(protocol, accountId);
		if (account)
			addMapping(id, account);
	}
}

Account SqlAccountsMapping::accountById(int sqlId) const
{
	if (AccountMapping.contains(sqlId))
		return AccountMapping.value(sqlId);
	else
		return Account::null;
}

int SqlAccountsMapping::idByAccount(const Account &account)
{
	return account.property("sql_history:id", 0).toInt();
}
