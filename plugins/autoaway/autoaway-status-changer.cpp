/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2010 badboy (badboy@gen2.org)
 * Copyright 2009, 2009, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "autoaway.h"

#include "autoaway-status-changer.h"

AutoAwayStatusChanger::AutoAwayStatusChanger(AutoAway *autoawayController, QObject *parent) :
		StatusChanger(900, parent), AutoawayController(autoawayController)
{
}

AutoAwayStatusChanger::~AutoAwayStatusChanger()
{
}

void AutoAwayStatusChanger::changeStatus(StatusContainer *container, Status &status)
{
	Q_UNUSED(container)

	ChangeStatusTo changeStatusTo = AutoawayController->changeStatusTo();
	ChangeDescriptionTo changeDescriptionTo = AutoawayController->changeDescriptionTo();
	QString descriptionAddon = AutoawayController->descriptionAddon();

	if (changeStatusTo == NoChangeStatus)
		return;

	if (status.isDisconnected())
		return;

	QString description = status.description();
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
		status.setType(StatusTypeOffline);
		status.setDescription(description);
		return;
	}

	if (status.group() == StatusTypeGroupInvisible)
		return;

	if (changeStatusTo == ChangeStatusToInvisible)
	{
		status.setType(StatusTypeInvisible);
		status.setDescription(description);
		return;
	}

	if (status.group() == StatusTypeGroupAway)
		return;

	if (changeStatusTo == ChangeStatusToAway)
	{
		status.setType(StatusTypeAway);
		status.setDescription(description);
		return;
	}

	if (changeStatusTo == ChangeStatusToExtendedAway)
	{
		status.setType(StatusTypeNotAvailable);
		status.setDescription(description);
		return;
	}
}

void AutoAwayStatusChanger::update()
{
	emit statusChanged(0); // for all status containers
}

#include "moc_autoaway-status-changer.cpp"
