/*
 * %kadu copyright begin%
 * Copyright 2009, 2011, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef STATUS_CHANGER_MANAGER_H
#define STATUS_CHANGER_MANAGER_H

#include <QtCore/QMap>

#include "status/status.h"

class Account;
class StatusChanger;
class StatusContainer;

/**
 * @addtogroup Status
 * @{
 */

/**
 * @class StatusChangerManager
 * @author Rafał 'Vogel' Malinowski
 * @short Class responsible for managing list of StatusChanger instnaces.
 *
 * Every Status in Kadu can be modified by plugins that registers instances of StatusChanger class in StatusChangerManager
 * singleton.
 *
 * For example: media player plugin can register StatusChanger that adds title of currenly playing song at the beggining
 * or at the end of user set description. Autoaway plugin can register StatusChanger that lowers availability depending
 * on user activity (like chaning status from online to away or do not distrurb or even offline).
 *
 * Using StatusChanger classes instead of directly changing status allows for easy stacking of plugins that can modify
 * status and for easy rewerting to user set status and description. Modifications of every StatusChanger are applied
 * in order provided by theirs priorities.
 *
 * To get modifies status use setStatus method of StatusChangerManager instead of StatusContainer one. This will create
 * modified status and set it on StatusContainer. Every change of modification of any registered StatusChanger will
 * be automatically applied on registered status containers.
 *
 * StatusChangerManager also allows receiving originally set status (not modified) by manuallySetStatus method.
 */
class KADUAPI StatusChangerManager : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(StatusChangerManager)

	static StatusChangerManager *Instance;

	QMap<StatusContainer *, Status> Statuses;
	QList<StatusChanger *> StatusChangers;

	StatusChangerManager();
	virtual ~StatusChangerManager();

private slots:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Slot called when any of registered StatusChanger changes way it modifies status.
	 * @param statusContainer change is touching this status container's status
	 *
	 * This slot called when any of registered StatusChanger changes way it modifies status for given
	 * statusContainer. If statusContainer is null, status for all registered status container is
	 * modified.
	 *
	 * After computing new status for given statusContainer, its setStatus method is called.
	 */
	void statusChanged(StatusContainer *statusContainer = 0);

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns StatusChangerManager singleton instance.
	 * @return StatusChangerManager singleton instance
	 *
	 * Returns StatusChangerManager singleton instance.
	 */
	static StatusChangerManager * instance();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Registers new StatusChanger object.
	 * @param statusChanger new statusChanger to register.
	 *
	 * Registers new StatusChanger instance. Calling this method causes recomputation of all status contaieners' statuses.
	 */
	void registerStatusChanger(StatusChanger *statusChanger);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Unregisters StatusChanger object.
	 * @param statusChanger statusChanger to unregister.
	 *
	 * Unregisters StatusChanger instance. Calling this method causes recomputation of all status contaieners' statuses.
	 */
	void unregisterStatusChanger(StatusChanger *statusChanger);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Sets status on given status container with modifications.
	 * @param statusContainer StatusContainer to set status on.
	 * @param status status to be modified and then set on statusContainer
	 *
	 * This methods stores given status as manually set status (to receive by manuallySetStatus) and then modifies
	 * it using all registered StatusChangers. After all changes are done, new status is set on statusContainer
	 * using StatusContainer::setStatus method.
	 */
	void setStatus(StatusContainer *statusContainer, Status status);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns manually (unchanged) status from given StatusContainer.
	 * @param statusContainer StatusContainer to get status from.
	 *
	 * This methods returns manually set status from given StatusContainer. This is the last status that was
	 * set using StatusChangerManager::setStatus method.
	 */
	Status manuallySetStatus(StatusContainer *statusContainer);

};

/**
 * @addtogroup Status
 * @}
 */

#endif // STATUS_CHANGER_MANAGER_H
