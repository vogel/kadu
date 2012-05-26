/*
 * %kadu copyright begin%
 * Copyright 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include "icons/icons-manager.h"
#include "icons/kadu-icon.h"
#include "protocols/protocol.h"
#include "status/status-container-manager.h"
#include "status/status-container.h"
#include "status/status-setter.h"
#include "status/status-type-data.h"
#include "status/status-type-group.h"
#include "status/status-type-manager.h"
#include "status/status-type.h"

#include "status-actions.h"

StatusActions::StatusActions(StatusContainer *statusContainer, bool includePrefix, QObject *parent) :
		QObject(parent), MyStatusContainer(statusContainer), IncludePrefix(includePrefix), ChangeDescription(0)
{
	ChangeStatusActionGroup = new QActionGroup(this);
	ChangeStatusActionGroup->setExclusive(true); // HACK
	connect(ChangeStatusActionGroup, SIGNAL(triggered(QAction*)), this, SIGNAL(statusActionTriggered(QAction*)));

	statusUpdated();
	connect(MyStatusContainer, SIGNAL(statusUpdated()), this, SLOT(statusUpdated()));

	connect(IconsManager::instance(), SIGNAL(themeChanged()), this, SLOT(iconThemeChanged()));
}

StatusActions::~StatusActions()
{
}

void StatusActions::createActions()
{
	createBasicActions();

	MyStatusTypes = MyStatusContainer->supportedStatusTypes();
	StatusTypeGroup currentGroup = StatusTypeGroupNone;
	bool setDescriptionAdded = false;

	foreach (StatusType statusType, MyStatusTypes)
	{
		if (StatusTypeNone == statusType)
			continue;

		const StatusTypeData & typeData = StatusTypeManager::instance()->statusTypeData(statusType);

		if (StatusTypeGroupNone == currentGroup)
			currentGroup = typeData.typeGroup();

		if (!setDescriptionAdded && typeData.typeGroup() == StatusTypeGroupOffline)
		{
			if (!Actions.isEmpty())
				Actions.append(createSeparator());
			Actions.append(ChangeDescription);
			setDescriptionAdded = true;
		}

		if (typeData.typeGroup() != currentGroup)
		{
			Actions.append(createSeparator());
			currentGroup = typeData.typeGroup();
		}

		QAction *action = createStatusAction(typeData);
		Actions.append(action);
	}

	emit statusActionsRecreated();
}

void StatusActions::createBasicActions()
{
	ChangeDescription = new QAction(tr("Change Status Message..."), this);
	connect(ChangeDescription, SIGNAL(triggered(bool)), this, SIGNAL(changeDescriptionActionTriggered(bool)));
}

QAction * StatusActions::createSeparator()
{
	QAction *separator = new QAction(this);
	separator->setSeparator(true);

	return separator;
}

QAction * StatusActions::createStatusAction(const StatusTypeData &typeData)
{
	KaduIcon icon = MyStatusContainer->statusIcon(typeData.type());
	QAction *statusAction = ChangeStatusActionGroup->addAction(icon.icon(), IncludePrefix
			? MyStatusContainer->statusNamePrefix() + typeData.displayName()
			: typeData.displayName());
	statusAction->setCheckable(true);
	statusAction->setData(QVariant::fromValue(typeData.type()));

	return statusAction;
}

void StatusActions::cleanUpActions()
{
	foreach (QAction *action, Actions)
		if (action != ChangeDescription)
		{
			if (!action->isSeparator())
				ChangeStatusActionGroup->removeAction(action);

			delete action;
		}

	Actions.clear();

	delete ChangeDescription;
	ChangeDescription = 0;
}

void StatusActions::statusUpdated()
{
	if (MyStatusContainer->supportedStatusTypes() != MyStatusTypes)
	{
		cleanUpActions();
		createActions();
	}

	StatusType currentStatusType = StatusSetter::instance()->manuallySetStatus(MyStatusContainer).type();
	if (!MyStatusContainer->supportedStatusTypes().contains(currentStatusType))
		currentStatusType = MyStatusContainer->status().type();

	foreach (QAction *action, ChangeStatusActionGroup->actions())
	{
		StatusType statusType = action->data().value<StatusType>();
		if (StatusTypeNone == statusType)
			continue;

		action->setIcon(MyStatusContainer->statusIcon(statusType).icon());

		if (!MyStatusContainer->isStatusSettingInProgress())
		{
			// For 'All xxx' status menu items - check only if all accounts have the same status
			if (StatusContainerManager::instance() == MyStatusContainer)
				action->setChecked(StatusContainerManager::instance()->allStatusOfType(statusType));
			else
				action->setChecked(currentStatusType == statusType);
		}
		else
			action->setChecked(false);
	}
}

void StatusActions::iconThemeChanged()
{
	foreach (QAction *action, ChangeStatusActionGroup->actions())
	{
		StatusType statusType = action->data().value<StatusType>();
		if (StatusTypeNone == statusType)
			continue;

		action->setIcon(MyStatusContainer->statusIcon(statusType).icon());
	}
}
