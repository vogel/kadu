/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef STATUS_CONTAINER_MANAGER_H
#define STATUS_CONTAINER_MANAGER_H

#include "accounts/accounts-aware-object.h"
#include "identities/identities-aware-object.h"
#include "status/status-container.h"

class AllAccountsStatusContainer;

/**
 * @addtogroup Status
 * @{
 */

/**
 * @class StatusContainerManager
 * @author Rafał 'Vogel' Malinowski
 * @short Class responsible for managing list of active status container.
 * @see StatusContainer
 * @see StatusContainerAwareObject
 *
 * This class is responsible for managing list of active status containers. For current Kadu implementation
 * this list can be either:
 * <ul>
 *   <li>list of all active accounts</li>
 *   <li>list of all active identities (that have at least one active account)</li>
 *   <li>one status container that contains all active accounts</li>
 * </ul>
 *
 * This class is used by status changing widgets and windows, like StatusMenu and ChooseDescription.
 *
 * StatusContainerManager is also a status container that delegates all of its getters to default status container (first
 * registered) and setters to all active status containers.
 *
 * Each class of type StatusContainerAwareObject is notified about registered and unregistered status contianers.
 */
class KADUAPI StatusContainerManager : public StatusContainer, private AccountsAwareObject, private IdentitiesAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(StatusContainerManager)

	static StatusContainerManager *Instance;

	QList<StatusContainer *> StatusContainers;
	StatusContainer *DefaultStatusContainer;
	AllAccountsStatusContainer *AllAccountsContainer;

	StatusContainerManager();
	virtual ~StatusContainerManager();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Unregister all status containers.
	 *
	 * This method unregister all status containers. After that, default status container is null.
	 */
	void cleanStatusContainers();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Adds all active accounts as status containers.
	 *
	 * This method adds all active accounts as status containers.
	 */
	void addAllAccounts();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Adds all active identities that have at least one account as status containers.
	 *
	 * This method adds all active identities that have at least one account as status containers. Work is done
	 * by calling updateIdentities slot.
	 */
	void addAllIdentities();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Sets new default status container.
	 * @param defaultStatusContainer new default status container
	 *
	 * This method sets new default status container.
	 */
	void setDefaultStatusContainer(StatusContainer *defaultStatusContainer);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Registers new status container.
	 * @param statusContainer new status container
	 *
	 * This method registers new status container. Signal statusUpdated of this container is connected to the same signal of
	 * StatusContainerManager. Signals statusContainerAboutToBeRegistered and statusContainerRegistered are emited. New
	 * status container can became default status container, if no one was available.
	 */
	void registerStatusContainer(StatusContainer *statusContainer);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Unregisters status container.
	 * @param statusContainer status container to unregister
	 *
	 * This method unregisters status container. Signal statusUpdated of this container is disconnected from the same signal of
	 * StatusContainerManager. Signals statusContainerAboutToBeUnregistered and statusContainerUnregistered are emited. New default
	 * status container may be set, if statusContainer was the default one. Empty default status container may be set, if no more
	 * active status containers are available.
	 */
	void unregisterStatusContainer(StatusContainer *statusContainer);

private slots:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Method called when identity data needs update.
	 *
	 * This method is called every time identity data has changed - new identity is registered, old one is registered, an account
	 * changed its identity or so on. When StatusContainerManager is in one container per identity mode this methods updates list
	 * of active status containers to contain only identities with at least one account.
	 */
	void updateIdentities();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Slot called when mode of StatusContainerManager is changed.
	 *
	 * This method is called every time StatusContainerManager mode is called. Three modes are available:
	 * <ul>
	 *   <li>list of all active accounts</li>
	 *   <li>list of all active identities (that have at least one active account)</li>
	 *   <li>one status container that contains all active accounts</li>
	 * </ul>
	 *
	 * After calling this method all status containers are unregistered and new ones are registered depending on mode.
	 */
	void setStatusModeChanged();

protected:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Slot called when new account is registered.
	 * @param account registed account
	 *
	 * Slot called when new account is registered. This slot allows for updating list of active status containers when
	 * StatusContainerManager is in one container per account mode. In one container per identity mode, updateIdentities
	 * method is called to ensure that no empty identity has active status container.
	 */
	virtual void accountRegistered(Account account);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Slot called when account is unregistered.
	 * @param account unregisted account
	 *
	 * Slot called when account is unregistered. This slot allows for updating list of active status containers when
	 * StatusContainerManager is in one container per account mode. In one container per identity mode, updateIdentities
	 * method is called to ensure that no empty identity has active status container.
	 */
	virtual void accountUnregistered(Account account);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Slot called when new identity is registered.
	 * @param identity registed identity
	 *
	 * Slot called when new identity is registered. This slot allows for updating list of active status containers when
	 * StatusContainerManager is in one container per identity mode.
	 */
	virtual void identityAdded(Identity identity);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Slot called when identity is unregistered.
	 * @param identity unregisted identity
	 *
	 * Slot called when identity is unregistered. This slot allows for updating list of active status containers when
	 * StatusContainerManager is in one container per identity mode.
	 */
	virtual void identityRemoved(Identity identity);

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns singleton instance of StatusContainerManager.
	 * @return singleton instance of StatusContainerManager
	 *
	 * Returns singleton instance of StatusContainerManager.
	 */
	static StatusContainerManager * instance();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns list of active status contianers.
	 * @return list of active status contianers
	 *
	 * Returns list of active status contianers.
	 */
	const QList<StatusContainer *> & statusContainers() const { return StatusContainers; }

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns true if statuses of all acvite status containers are of given type.
	 * @return true if statuses of all acvite status containers are of given type
	 *
	 * Returns true if statuses of all acvite status containers are of given type.
	 */
	bool allStatusOfType(StatusType type);

	// StatusContainer Implementation

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns "All" string.
	 * @return "All" string
	 *
	 * Returns "All" string.
	 */
	virtual QString statusContainerName();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Sets new status on all active status containers.
	 * @param status new status
	 *
	 * Sets new status on all active status containers.
	 */
	virtual void setStatus(Status status);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns status of default status container.
	 * @return status of default status container
	 *
	 * Returns status of default status container. If no default status container is available, returns "Offline" status.
	 */
	virtual Status status();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns "is status setting in progress" property of default status container.
	 * @return "is status setting in progress" property of default status container
	 *
	 * Returns "is status setting in progress" property of default status container. If no default status container is available, returns false.
	 */
	virtual bool isStatusSettingInProgress();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns generic icon of status of default status container.
	 * @return generic icon of status of default status container
	 *
	 * Returns generic (without protocol badge) icon of status of default status container. If no default status container is available,
	 * returns "Offline" icon.
	 */
	virtual KaduIcon statusIcon();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns generic icon of given status.
	 * @param status status to get icon for
	 * @return generic icon of given status
	 *
	 * Returns generic (without protocol badge) icon of given status.
	 */
	virtual KaduIcon statusIcon(const Status &status);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns list of default status container supported status types.
	 * @return list of default status container supported status types
	 *
	 * Returns list of default status container supported status types. If no default status container is available, empty list is returned.
	 */
	virtual QList<StatusType> supportedStatusTypes();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns maximum description length of default status container.
	 * @return maximum description length of default status container
	 *
	 * Returns maximum description length of default status container. If no default status container is available, -1 is returned.
	 */
	virtual int maxDescriptionLength();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns "All " string.
	 * @return "All " string
	 *
	 * Returns "All " string.
	 */
	virtual QString statusNamePrefix();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns stored status of default status container.
	 * @return stored status of default status container
	 *
	 * Returns stored status of default status container.If no default status container is available, "Offline" status is returned.
	 */
	virtual Status loadStatus();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Stores given status in all active status containers.
	 * @param status status to store
	 *
	 * Stores given status in all active status containers.
	 */
	virtual void storeStatus(Status status);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns list of sub status containers.
	 * @return list of sub status containers
	 *
	 * Returns list of all registered status containers - that can be list of accounts, identities or an
	 * instance of AllAccountsStatusContainer.
	 */
	virtual QList<StatusContainer *> subStatusContainers();

signals:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Signal emited just before new status container is registered.
	 * @param statusContainer new status container
	 *
	 * Signal emited just before new status container is registered
	 */
	void statusContainerAboutToBeRegistered(StatusContainer *statusContainer);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Signal emited just after new status container is registered.
	 * @param statusContainer new status container
	 *
	 * Signal emited just after new status container is registered
	 */
	void statusContainerRegistered(StatusContainer *statusContainer);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Signal emited just before status container is unregistered.
	 * @param statusContainer unregistered status container
	 *
	 * Signal emited just before new status container is unregistered
	 */
	void statusContainerAboutToBeUnregistered(StatusContainer *statusContainer);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Signal emited just after status container is unregistered.
	 * @param statusContainer unregistered status container
	 *
	 * Signal emited just after new status container is unregistered
	 */
	void statusContainerUnregistered(StatusContainer *statusContainer);

};

/**
 * @}
 */

#endif // STATUS_CONTAINER_MANAGER_H
