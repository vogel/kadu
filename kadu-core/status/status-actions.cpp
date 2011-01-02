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

#include "accounts/account-manager.h"
#include "configuration/configuration-file.h"
#include "protocols/protocol.h"
#include "status/status-container.h"
#include "status/status-container-manager.h"
#include "status/status-group.h"
#include "status/status-type.h"

#include "status-actions.h"

StatusActions::StatusActions(StatusContainer *statusContainer, QObject *parent, bool commonStatusIcons) :
		QObject(parent), MyStatusContainer(statusContainer), CommonStatusIcons(commonStatusIcons)
{
	ChangeStatusActionGroup = new QActionGroup(this);
	ChangeStatusActionGroup->setExclusive(true); // HACK
	connect(ChangeStatusActionGroup, SIGNAL(triggered(QAction*)), this, SIGNAL(statusActionTriggered(QAction*)));

	createActions();

	statusChanged();
	connect(MyStatusContainer, SIGNAL(statusChanged()), this, SLOT(statusChanged()));
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
}

void StatusActions::createBasicActions()
{
	ChangeDescription = new QAction(tr("Change Status Message..."), this);
	connect(ChangeDescription, SIGNAL(triggered(bool)), this, SIGNAL(changeDescriptionActionTriggered(bool)));
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
	QIcon icon;
	if (!CommonStatusIcons)
		icon = MyStatusContainer->statusIcon(statusType->name());
	else
		icon = StatusContainerManager::instance()->statusIcon(statusType->name());
	QAction *statusAction = ChangeStatusActionGroup->addAction(
		icon.pixmap(16, 16),
		MyStatusContainer->statusNamePrefix() + statusType->displayName());
	statusAction->setCheckable(true);
	statusAction->setData(QVariant::fromValue(statusType));

	return statusAction;
}

void StatusActions::statusChanged()
{
	const QString &statusTypeName = MyStatusContainer->status().type();

	foreach (QAction *action, ChangeStatusActionGroup->actions())
	{
		StatusType *statusType = action->data().value<StatusType *>();
		if (!statusType)
			continue;

		// For 'All xxx' status menu items - check only if all accounts have the same status
		if(StatusContainerManager::instance() == MyStatusContainer)
			action->setChecked(StatusContainerManager::instance()->allStatusEqual(statusType));
		else
			action->setChecked(statusTypeName == statusType->name());
	}

// 	ChangeStatusToOfflineDesc->setEnabled(index != 6);
// 	ChangeStatusToOffline->setEnabled(index != 7);
}
