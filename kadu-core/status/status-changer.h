/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef STATUS_CHANGER_H
#define STATUS_CHANGER_H

#include <QtCore/QObject>

#include "exports.h"

class Status;
class StatusContainer;

/**
 * @addtogroup Status
 * @{
 */

/**
 * @class StatusChanger
 * @author Rafał 'Vogel' Malinowski
 * @short Class responsible for changing status.
 * @see StatusChangerManager
 *
 * This class is responsible for changing user status according to some rules. For example, implementation of this class
 * from media player plugin can add title of current song before or after user set description. Implemenation from
 * autoaway plugin can lower availability of current status depending of user activity.
 *
 * All instances of StatusChanger subclasses are registered in StatusChangerManager. Their changeStatus methods
 * are called in order depending of priority (lower number of priority means that this class will be used first).
 *
 * Every StatusChanger can emit statusChanged signal to inform StatusChangerManager that status must be recalculated,
 * because some conditions may have changed (like currently playing song or user activity).
 */
class KADUAPI StatusChanger : public QObject
{
	Q_OBJECT

	int Priority;

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Creates new instance of StatusChanger.
	 * @param priority priority of given StatusChanger
	 * @param parent parent of given StatusChanger
	 *
	 * Creates new instance of StatusChanger. Lower number of priority means that this class will be used first in
	 * status calculations.
	 */
	explicit StatusChanger(int priority, QObject *parent = 0);
	virtual ~StatusChanger();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns priority of this StatusChanger.
	 * @return priority of this StatusChanger
	 *
	 * Returns priority of this StatusChanger. Lower number of priority means that this class will be used first in
	 * status calculations.
	 */
	int priority();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Method called to change given status.
	 * @param statusContainer status container of given status
	 * @param status status to change
	 *
	 * Reimplement this method to change status in any way. Status is passed as reference for performance reasons.
	 */
	virtual void changeStatus(StatusContainer *statusContainer, Status &status) = 0;

signals:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Signal emited when status for given status container needs recalculation.
	 * @param statusContainer container that requires status recalculation
	 *
	 * Emit this status when StatusChangerManager should recalculate status for given container (for example
	 * when currently playing song changed or user activity changed). Use 0 for statusContainer if all
	 * statuses needs to be recalculated.
	 */
	void statusChanged(StatusContainer *statusContainer);

};

/**
 * @}
 */

#endif // STATUS_CHANGER_H
