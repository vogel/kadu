/*
 * %kadu copyright begin%
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "status/status-changer.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class Autoaway;

class AutoawayStatusChanger : public StatusChanger
{
	Q_OBJECT

	friend class Autoaway;

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

	Q_INVOKABLE explicit AutoawayStatusChanger(QObject *parent = nullptr);
	virtual ~AutoawayStatusChanger();

	virtual void changeStatus(StatusContainer *container, Status &status);

	void update();

private:
	QPointer<Autoaway> m_autoaway;

private slots:
	INJEQT_SETTER void setAutoaway(Autoaway *autoaway);

};
