/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef STATUS_CONTAINER_H
#define STATUS_CONTAINER_H

#include "status/status-change-source.h"
#include "status/status-type.h"
#include "status/status.h"
#include "exports.h"

class KaduIcon;

/**
 * @addtogroup Status
 * @{
 */

/**
 * @class StatusContainer
 * @author Rafał 'Vogel' Malinowski
 * @short Class responsible for managing status of account or group of accounts.
 * @see StatusContainerManager
 *
 * This class is responsible for managing status of account or group of accounts. Use objects of this class
 * to get or set statuses for accounts. StatusContainerManager contains list of current status containers
 * that depends of user setting (one container per account, per identity or one container at all).
 *
 * StatusContainer contains methods for fetching status icons, max description length and list of status types
 * common for all accounts/identities under given status container.
 */
class KADUAPI StatusContainer : public QObject
{
	Q_OBJECT

	// all status changes must be performed by Core
	friend class Core;

public:
	explicit StatusContainer(QObject *parent = 0);

	virtual ~StatusContainer();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns display name of status container.
	 * @return display name of status container
	 *
	 * This method returns display name of status container. This name is displayed to user.
	 */
	virtual QString statusContainerName() = 0;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Sets status for this status container.
	 * @param status status to set
	 * @param source source of status change
	 * @see StatusSetter
	 * @see StatusChangerManager
	 *
	 * This method set status for current status container. All accounts contained by this status
	 * container will have this status set. This may require login or logout of these accounts.
	 *
	 * Use StatusSetter class to setting status if this status should be changed by StatusChanger
	 * classes.
	 */
	virtual void setStatus(Status status, StatusChangeSource source) = 0;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Return current status of this status container.
	 * @return current status of this status container
	 * @see StatusSetter
	 * @see StatusChangerManager
	 *
	 * This method return current status of this status container. If this container does not have any
	 * account, Offline status is returned. If this container contains more than one account, most available
	 * status is returned. If this container contains exactly one account, status for this account is
	 * returned.
	 *
	 * Please note that this status may be different than status set by user due to StatusChanger mechanism.
	 * Use StatusSetter::manuallySetStatus to get status set by user.
	 */
	virtual Status status() = 0;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns true if status is currently being set.
	 * @return true if status is currently being set
	 *
	 * This method returns true if status is currently being set - it should only occur on login, as when
	 * connection is established status changes are performed immediately by most protocols. This information
	 * is used to determine if status icons should be blinking.
	 */
	virtual bool isStatusSettingInProgress() = 0;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns icon for this container status.
	 * @return icon for this container status
	 *
	 * This method returns icon for status returned by status method.
	 */
	virtual KaduIcon statusIcon() = 0;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns icon for given status
	 * @param status status to get icon for
	 * @return icon for given status
	 *
	 * This method returns icon for given status. Icons returned by different StatusContainers for the
	 * same status can be different, as Kadu supports per-protocol icons. One-account status containers
	 * use per-protocol icons.
	 */
	virtual KaduIcon statusIcon(const Status &status) = 0;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns list of supported status types.
	 * @return list of supported status types
	 *
	 * This method returns icon for given status type. Icons returned by different StatusContainers for the
	 * same status type can be different, as Kadu supports per-protocol icons. One-account status containers
	 * use per-protocol icons. This icon does not have description mark.
	 *
	 * For multiaccount containers intersection of status types supported by its protocols is returned.
	 */
	virtual QList<StatusType> supportedStatusTypes() = 0;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns maximum length of description.
	 * @return maximum length of description
	 *
	 * This method returns maximum length of description.
	 *
	 * For multiaccount containers minimum value is returned.
	 */
	virtual int maxDescriptionLength() = 0;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns prefix used in status menus.
	 * @return prefix used in status menus
	 *
	 * Returns prefix used in status menus. Used by main status container to display "All" in docking menu.
	 */
	virtual QString statusNamePrefix() { return QString(); }

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Return stored status.
	 * @return stored status.
	 *
	 * Return stored status.
	 */
	virtual Status loadStatus() = 0;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Stores given status.
	 * @param status status to store
	 *
	 * Stores given status.
	 */
	virtual void storeStatus(Status status) = 0;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns list of sub status containers.
	 * @return list of sub status containers
	 *
	 * Status containers can contain other status containers. This method returns list of real status
	 * containers for given aggregator. For now all implementations but StatusContainerManager returns list
	 * containing only one object - this object.
	 *
	 * StatusContainerManager returns list of all registered status containers - that can be list of accounts,
	 * identities or an instance of AllAccountsStatusContainer.
	 */
	virtual QList<StatusContainer *> subStatusContainers();

signals:
	/**
	 * @short Signal emited when status was changed.
	 *
	 * This signal is emitted when the status was explicitly changed or something in this
	 * StatusContainer was changed that might have caused implicit status change (but did not
	 * need to, so do not assume real status change after this signal).
	 */
	void statusUpdated(StatusContainer *container);

};

/**
 * @}
 */

#endif // STATUS_CONTAINER_H
