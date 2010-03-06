/*
 * %kadu copyright begin%
 * Copyright 2009, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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
#include <QtGui/QActionGroup>
#include <QtGui/QMenu>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "configuration/configuration-file.h"
#include "core/core.h"
#include "gui/windows/choose-description.h"
#include "protocols/protocol.h"
#include "status/status-group.h"
#include "status/status-type.h"
#include "status/status-type-manager.h"

#include "status-menu.h"

StatusMenu::StatusMenu(StatusContainer *statusContainer, QMenu *menu) :
		QObject(menu), Menu(menu), MyStatusContainer(statusContainer)
{
	ChangeStatusActionGroup = new QActionGroup(this);
	ChangeStatusActionGroup->setExclusive(true); // HACK

	connect(ChangeStatusActionGroup, SIGNAL(triggered(QAction *)), this, SLOT(changeStatus(QAction *)));

	ChangeDescription = new QAction(tr("Change status message..."), this);
	connect(ChangeDescription, SIGNAL(triggered(bool)), this, SLOT(changeDescription()));

	ChangePrivateStatus = new QAction(tr("Private"), this);
	ChangePrivateStatus->setCheckable(true);
	connect(ChangePrivateStatus, SIGNAL(toggled(bool)), this, SLOT(changeStatusPrivate(bool)));

	bool privateStatus = config_file.readBoolEntry("General", "PrivateStatus");
	ChangePrivateStatus->setChecked(privateStatus);

	statusChanged();
	connect(MyStatusContainer, SIGNAL(statusChanged()), this, SLOT(statusChanged()));
	connect(MyStatusContainer, SIGNAL(updated()), this, SLOT(statusContainerUpdated()));

	createActions();
	connect(Menu, SIGNAL(aboutToHide()), this, SLOT(aboutToHide()));
}

StatusMenu::~StatusMenu()
{
	disconnect(MyStatusContainer, SIGNAL(statusChanged()), this, SLOT(statusChanged()));
}

void StatusMenu::clearActions()
{
	while (!ChangeStatusActionGroup->actions().isEmpty())
	{
		QAction *action = ChangeStatusActionGroup->actions().first();
		ChangeStatusActionGroup->removeAction(action);
		Menu->removeAction(action);
		action->deleteLater();
	}

	while (!MenuActions.isEmpty())
	{
		QAction *action = MenuActions.first();
		Menu->removeAction(action);
		action->deleteLater();
	}

	Menu->removeAction(ChangeDescription);
	Menu->removeAction(ChangePrivateStatus);
}

void StatusMenu::createStatusActions()
{
	const QString &statusTypeName = MyStatusContainer->status().type();

	QList<StatusType *> statusTypes = MyStatusContainer->supportedStatusTypes();
	foreach (StatusType *statusType, statusTypes)
	{
		QAction *statusAction = ChangeStatusActionGroup->addAction(
				MyStatusContainer->statusPixmap(statusType->name()),
				MyStatusContainer->statusNamePrefix() + statusType->displayName());
		statusAction->setCheckable(true);
		statusAction->setData(QVariant::fromValue(statusType));

		if (statusTypeName == statusType->name())
			statusAction->setChecked(true);
	}
}

void StatusMenu::createActions()
{
	createStatusActions();

	if (0 == ChangeStatusActionGroup->actions().count())
		return;

	StatusType *statusType = ChangeStatusActionGroup->actions()[0]->data().value<StatusType *>();
	if (0 == statusType)
		return;

	StatusGroup *currentGroup = statusType->statusGroup();
	bool setDescriptionAdded = false;

	foreach (QAction *action, ChangeStatusActionGroup->actions())
	{
		StatusType *statusType = action->data().value<StatusType *>();
		if (0 == statusType)
			return;

		if (!setDescriptionAdded && statusType->statusGroup() &&
				statusType->statusGroup()->sortIndex() >= StatusGroup::StatusGroupSortIndexAfterSetDescription)
		{
			MenuActions.append(Menu->addSeparator());
			Menu->addAction(ChangeDescription);
		}

		if (statusType->statusGroup() != currentGroup)
		{
			MenuActions.append(Menu->addSeparator());
			currentGroup = statusType->statusGroup();
		}

		Menu->addAction(action);
	}

	MenuActions.append(Menu->addSeparator());
	Menu->addAction(ChangePrivateStatus);
}

void StatusMenu::aboutToHide()
{
	MousePositionBeforeMenuHide = Menu->pos();
}

void StatusMenu::changeStatus(QAction *action)
{
	StatusType *statusType = action->data().value<StatusType *>();
	if (!statusType)
		return;

	Status status(MyStatusContainer->status());
	status.setType(statusType->name());
	MyStatusContainer->setStatus(status);
}

void StatusMenu::changeDescription()
{
	ChooseDescription::showDialog(MyStatusContainer, MousePositionBeforeMenuHide);
}

void StatusMenu::changeStatusPrivate(bool toggled)
{
	if (AccountManager::instance()->defaultAccount().protocolHandler())
		AccountManager::instance()->defaultAccount().protocolHandler()->setPrivateMode(toggled);

	config_file.writeEntry("General", "PrivateStatus", toggled);
}

void StatusMenu::statusChanged()
{
	const QString &statusTypeName = MyStatusContainer->status().type();

	foreach (QAction *action, ChangeStatusActionGroup->actions())
	{
		StatusType *statusType = action->data().value<StatusType *>();
		if (!statusType)
			continue;

		action->setChecked(statusTypeName == statusType->name());
	}

	if (!AccountManager::instance()->defaultAccount().isNull())
	{
		Protocol *protocol = AccountManager::instance()->defaultAccount().protocolHandler();
		if (!protocol)
			return;
		ChangePrivateStatus->setChecked(protocol->privateMode());
	}

// 	ChangeStatusToOfflineDesc->setEnabled(index != 6);
// 	ChangeStatusToOffline->setEnabled(index != 7);
}

void StatusMenu::statusContainerUpdated()
{
	printf("scu called\n");

	clearActions();
	createActions();
}
