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

#include <QtGui/QAction>

#include "status/status-container.h"
#include "status/status-group.h"
#include "status/status-type.h"

#include "status-actions.h"

StatusActions::StatusActions(StatusContainer *statusContainer, QObject *parent) :
		QObject(parent), MyStatusContainer(statusContainer)
{
	ChangeStatusActionGroup = new QActionGroup(this);
	ChangeStatusActionGroup->setExclusive(true); // HACK

	createActions();
}

StatusActions::~StatusActions()
{
}

void StatusActions::createActions()
{
	createBasicActions();
	createStatusActions();

	QList<StatusType *> statusTypes = MyStatusContainer->supportedStatusTypes();
	if (statusTypes.isEmpty())
		return;

	StatusType *statusType = statusTypes.first();
	if (0 == statusType)
		return;

	StatusGroup *currentGroup = statusType->statusGroup();
	bool setDescriptionAdded = false;

	foreach (StatusType *statusType, statusTypes)
	{
		if (0 == statusType)
			continue;

		if (!setDescriptionAdded && statusType->statusGroup() &&
				statusType->statusGroup()->sortIndex() >= StatusGroup::StatusGroupSortIndexAfterSetDescription)
		{
			Actions.append(createSeparator());
			Actions.append(ChangeDescription);
		}

		if (statusType->statusGroup() != currentGroup)
		{
			Actions.append(createSeparator());
			currentGroup = statusType->statusGroup();
		}

		Actions.append(StatusTypeActions[statusType]);
	}

	Actions.append(createSeparator());
	Actions.append(ChangePrivateStatus);
}

void StatusActions::createBasicActions()
{
	ChangeDescription = new QAction(tr("Change status message..."), this);
	ChangePrivateStatus = new QAction(tr("Private"), this);
	ChangePrivateStatus->setCheckable(true);
}

void StatusActions::createStatusActions()
{
	QList<StatusType *> statusTypes = MyStatusContainer->supportedStatusTypes();
	foreach (StatusType *statusType, statusTypes)
	{
		QAction *action = createStatusAction(statusType);
		StatusTypeActions.insert(statusType, action);
	}
}

QAction * StatusActions::createSeparator()
{
	QAction *separator = new QAction(this);
	separator->setSeparator(true);

	return separator;
}

QAction * StatusActions::createStatusAction(StatusType *statusType)
{
	QAction *statusAction = ChangeStatusActionGroup->addAction(
			MyStatusContainer->statusPixmap(statusType->name()),
			MyStatusContainer->statusNamePrefix() + statusType->displayName());
	statusAction->setCheckable(true);
	statusAction->setData(QVariant::fromValue(statusType));

	return statusAction;
}
