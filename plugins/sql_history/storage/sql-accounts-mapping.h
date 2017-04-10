/*
 * %kadu copyright begin%
 * Copyright 2012, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "accounts/accounts-aware-object.h"

#include <QtCore/QMap>
#include <QtCore/QMutex>
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtSql/QSqlDatabase>
#include <injeqt/injeqt.h>

class AccountManager;

/**
 * @addtogroup SqlHistory
 * @{
 */

/**
 * @class SqlAccountsMapping
 * @short This class is responsible for mapping IM accounts to database entries in kadu_accounts table.
 *
 * This class is responsible for mapping IM accounts to database entries in kadu_accounts table of database
 * provided in costructor. This class does not check for validity of database tables.
 *
 * Each account in application have its own mapping to entry in kadu_accounts table in databse. Added accounts
 * are added to database. Removed accounts have their data (protocol and account username) removed from database,
 * but id entry will be always available. Accounts updates (account username changes) are also reflected in database.
 *
 * Each accounts has custom property named sql_history:id that holds database id. If this property is not
 * positive integer, then given account does not have databse id (it is only possible if this account is not
 * available in @link AccountManager @endlink). Value of this property can be easily obtained by @link idByAccount
 * @endlink method.
 *
 * Also an account can be obtained from database id by calling @link accountById @endlink. This method can return
 * Account::null if no valid mapping exists for given id.
 */
class SqlAccountsMapping : public QObject, AccountsAwareObject
{
    Q_OBJECT

public:
    /**
     * @short Create new instance of SqlAccountsMapping class.
     * @param database database with kadu_accounts table
     * @param parent QObject parent of new SqlAccountsMapping object
     *
     * Database provided in this constructor is not checked for existence or validity of kadu_accounts table. It is
     * caller responsibility
     * to provide proper database.
     */
    explicit SqlAccountsMapping(const QSqlDatabase &database, QObject *parent = nullptr);
    virtual ~SqlAccountsMapping();

    /**
     * @short Return Account matched with given id from database.
     * @param sqlId id from database
     *
     * This method return an Account that matches with given id from database. If no matching account is found,
     * Account::null is returned.
     */
    Account accountById(int sqlId) const;

    /**
     * @short Return sql id matched with given account.
     * @param account account to match to sql id
     *
     * This method return sql id that matches with given account. If no matching id is found, 0 is returned.
     */
    static int idByAccount(const Account &account);

protected:
    /**
     * @short Add new kadu_accounts entry for new account.
     * @param account added account
     *
     * This slot is called every time an account is added in @link AccountManager @endlink. New entry is added to
     * database
     * to match data of new account.
     */
    virtual void accountAdded(Account account) override;

    /**
     * @short Clear kadu_accounts entry for removed account.
     * @param account added account
     *
     * This slot is called every time an account is removed from @link AccountManager @endlink. Entry for this account
     * is cleared
     * from database so no protocol or account username is remembered.
     */
    virtual void accountRemoved(Account account) override;

private:
    QPointer<AccountManager> m_accountManager;

    const QSqlDatabase &Database;
    mutable QMutex Mutex;
    QMap<int, Account> AccountMapping;

    /**
     * @short Add mapping of given id to given account.
     * @param id id to map
     * @param account account to map
     *
     * This method operates only on internal QMap and account's custom properties. No database changes
     * are performed.
     */
    void addMapping(int id, const Account &account);

    /**
     * @short Load mapping from database.
     *
     * This method loads mapping from database. All database entries from kadu_accounts table are matched
     * to existing accounts. Entries without valid accounts will be ignored by this method.
     */
    void loadMappingsFromDatabase();

private slots:
    INJEQT_SET void setAccountManager(AccountManager *accountManager);
    INJEQT_INIT void init();

    /**
     * @short Update kadu_accounts entry to new account's data.
     * @param account updated account
     *
     * This slot is called every time an account registered in @link AccountManager @endlink changes. Database data
     * is updated to match new data of changed account.
     */
    void accountUpdated(const Account &account);
};

/**
 * @}
 */
