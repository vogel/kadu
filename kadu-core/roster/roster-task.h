/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "exports.h"

#include <QtCore/QString>

enum class RosterTaskType;

/**
 * @addtogroup Roster
 * @{
 */

/**
 * @class RosterTask
 * @short Class describing one roster task.
 *
 * Roster task is described by contact id (username) and roster task type. More data about task is read from Kadu's
 * @link ContactManager @endlink singleton.
 */
class KADUAPI RosterTask
{

public:
	RosterTask();
	RosterTask(RosterTaskType type, const QString &id);

	RosterTaskType type() const;
	QString id() const;

	friend KADUAPI bool operator == (const RosterTask &x, const RosterTask &y);

private:
	RosterTaskType m_type;
	QString m_id;

};

/**
 * @}
 */
