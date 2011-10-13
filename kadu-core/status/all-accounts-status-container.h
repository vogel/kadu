/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef ALL_ACCOUNTS_STATUS_CONTAINER_H
#define ALL_ACCOUNTS_STATUS_CONTAINER_H

#include "accounts/accounts-aware-object.h"
#include "status/status-container.h"

class Account;

/**
 * @addtogroup Status
 * @{
 */

/**
 * @class AllAccountsStatusContainer
 * @author Rafał 'Vogel' Malinowski
 * @short Grouping status container for all accounts.
 * @see StatusContainerManager
 *
 * This class is status container wrapper for all registered accounts. All its getters are delegated to best account
 * status container (@see AccountManager::bestAccount). All its setters are delegated to all accounts.
 */
class KADUAPI AllAccountsStatusContainer : public StatusContainer, public AccountsAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(AllAccountsStatusContainer)

	QList<Account> Accounts;

protected:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Method called when new account is registered.
	 *
	 * This method is called every time new account is registered. New account is added to list of
	 * status containers handled by this object. Signal statusUpdated is emited, as this new account
	 * could change AccountManager::bestAccount and result of all getters results.
	 */
	virtual void accountRegistered(Account account);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Method called when account is unregistered.
	 *
	 * This method is called every time new account is unregistered. This account is removed from list of
	 * status containers handled by this object. Signal statusUpdated is emited, as removal of this account
	 * could change AccountManager::bestAccount and result of all getters results.
	 */
	virtual void accountUnregistered(Account account);

public:
	explicit AllAccountsStatusContainer(QObject *parent = 0);
	virtual ~AllAccountsStatusContainer();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns empty string as this container name.
	 * @return empty string as this container name
	 *
	 * Returns empty string as this container name.
	 */
	virtual QString statusContainerName()  { return QString(); }

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Sets status on all registered accounts.
	 * @param status status to set
	 *
	 * Sets status on all registered accounts.
	 */
	virtual void setStatus(Status status);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Return best account status.
	 * @return best account status
	 *
	 * Return best account status.
	 * For definition of best account see @see AccountManager::bestAccount.
	 */
	virtual Status status();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Return true if best account status setting is in progress.
	 * @return true if best account status setting is in progress
	 *
	 * Return true if best account status setting is in progress.
	 * For definition of best account see @see AccountManager::bestAccount.
	 */
	virtual bool isStatusSettingInProgress();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Return maximum length of description on best account.
	 * @return maximum length of description on best account
	 *
	 * Return maximum length of description on best account.
	 * For definition of best account see @see AccountManager::bestAccount.
	 */
	virtual int maxDescriptionLength();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Return status icon of best account.
	 * @return status icon of best accountt
	 *
	 * Return maximum length of description on best account.
	 * Return status icon of best account.
	 */
	virtual KaduIcon statusIcon();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Return status icon of given status using best account icons.
	 * @param status to get icon for
	 * @return status icon of given status using best account icons
	 *
	 * Return status icon of given status using best account icons.
	 * Return status icon of best account.
	 */
	virtual KaduIcon statusIcon(const Status &status);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Return list of supported status types for best account.
	 * @return list of supported status types for best account
	 *
	 * Return list of supported status types for best account.
	 * Return status icon of best account.
	 */
	virtual QList<StatusType> supportedStatusTypes();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Return best account stored status.
	 * @return best account stored status
	 *
	 * Return best account stored status.
	 * Return status icon of best account.
	 */
	virtual Status loadStatus();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Stores given status in all accounts.
	 * @param status status to store
	 *
	 * Stores given status in all accounts.
	 */
	virtual void storeStatus(Status status);

};

/**
 * @}
 */

#endif // ALL_ACCOUNTS_STATUS_CONTAINER_H
