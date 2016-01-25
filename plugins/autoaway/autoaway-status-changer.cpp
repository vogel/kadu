/*
 * %kadu copyright begin%
 * Copyright 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "status/status-type-manager.h"
#include "status/status.h"

#include "autoaway.h"

#include "autoaway-status-changer.h"

AutoawayStatusChanger::AutoawayStatusChanger(QObject *parent) :
		StatusChanger{900, parent}
{
}

AutoawayStatusChanger::~AutoawayStatusChanger()
{
}

void AutoawayStatusChanger::setAutoaway(Autoaway *autoaway)
{
	m_autoaway = autoaway;
}

void AutoawayStatusChanger::setStatusTypeManager(StatusTypeManager *statusTypeManager)
{
	m_statusTypeManager = statusTypeManager;
}

void AutoawayStatusChanger::changeStatus(StatusContainer *container, Status &status)
{
	Q_UNUSED(container)

	auto changeStatusTo = m_autoaway->changeStatusTo();
	auto changeDescriptionTo = m_autoaway->changeDescriptionTo();
	auto descriptionAddon = m_autoaway->descriptionAddon();

	if (changeStatusTo == NoChangeStatus)
		return;

	if (status.isDisconnected())
		return;

	auto description = status.description();
	switch (changeDescriptionTo)
	{
		case NoChangeDescription:
			break;

		case ChangeDescriptionPrepend:
			description = descriptionAddon + description;
			break;

		case ChangeDescriptionReplace:
			description = descriptionAddon;
			break;

		case ChangeDescriptionAppend:
			description = description + descriptionAddon;
			break;
	}

	if (changeStatusTo == ChangeStatusToOffline)
	{
		status.setType(m_statusTypeManager, StatusTypeOffline);
		status.setDescription(description);
		return;
	}

	if (status.group() == StatusTypeGroupInvisible)
		return;

	if (changeStatusTo == ChangeStatusToInvisible)
	{
		status.setType(m_statusTypeManager, StatusTypeInvisible);
		status.setDescription(description);
		return;
	}

	if (status.group() == StatusTypeGroupAway)
		return;

	if (changeStatusTo == ChangeStatusToAway)
	{
		status.setType(m_statusTypeManager, StatusTypeAway);
		status.setDescription(description);
		return;
	}

	if (changeStatusTo == ChangeStatusToExtendedAway)
	{
		status.setType(m_statusTypeManager, StatusTypeNotAvailable);
		status.setDescription(description);
		return;
	}
}

void AutoawayStatusChanger::update()
{
	emit statusChanged(0); // for all status containers
}

#include "moc_autoaway-status-changer.cpp"
