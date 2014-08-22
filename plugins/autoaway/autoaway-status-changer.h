/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 badboy (badboy@gen2.org)
 * Copyright 2009, 2009, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef AUTOAWAY_STATUS_CHANGER_H
#define AUTOAWAY_STATUS_CHANGER_H

#include "status/status-changer.h"

class AutoAway;

class AutoAwayStatusChanger : public StatusChanger
{
	Q_OBJECT

	friend class AutoAway;

public:
	enum ChangeStatusTo {
		NoChangeStatus,
		ChangeStatusToAway,
		ChangeStatusToExtendedAway,
		ChangeStatusToInvisible,
		ChangeStatusToOffline
	};

	enum ChangeDescriptionTo {
		NoChangeDescription,
		ChangeDescriptionReplace,
		ChangeDescriptionPrepend,
		ChangeDescriptionAppend
	};

private:
	AutoAway *AutoawayController;

public:
	explicit AutoAwayStatusChanger(AutoAway *autoawayController, QObject *parent = 0);
	virtual ~AutoAwayStatusChanger();

	virtual void changeStatus(StatusContainer *container, Status &status);

	void update();

};

#endif // AUTOAWAY_STATUS_CHANGER_H
