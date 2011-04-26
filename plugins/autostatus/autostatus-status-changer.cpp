/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "status/status.h"

#include "autostatus-status-changer.h"

AutostatusStatusChanger::AutostatusStatusChanger(QObject *parent) :
		StatusChanger(200, parent), Enabled(false)
{
}

AutostatusStatusChanger::~AutostatusStatusChanger()
{
}

void AutostatusStatusChanger::setEnabled(bool enabled)
{
	if (Enabled != enabled)
	{
		Enabled = enabled;
		emit statusChanged(0);
	}
}

void AutostatusStatusChanger::setConfiguration(int statusIndex, const QString &description)
{
	if (StatusIndex != statusIndex || Description != description)
	{
		StatusIndex = statusIndex;
		Description = description;
		if (Enabled)
			emit statusChanged(0);
	}
}

void AutostatusStatusChanger::changeStatus(StatusContainer *container, Status &status)
{
	Q_UNUSED(container)

	if (!Enabled)
		return;

	switch (StatusIndex)
	{
		case 0:
			status.setType("Online");
			break;
		case 1:
			status.setType("Away");
			break;
		case 2:
			status.setType("Invisible");
			break;
	}

	status.setDescription(Description);
}
