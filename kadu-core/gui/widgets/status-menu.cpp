/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

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

#include "status_changer.h"

#include "status-menu.h"

StatusMenu::StatusMenu(StatusContainer *statusContainer, QWidget *parent) :
		QObject(parent), MyStatusContainer(statusContainer)
{
	ChangeStatusActionGroup = new QActionGroup(this);
	ChangeStatusActionGroup->setExclusive(true); // HACK

	// TODO: 0.6.6

	const QString &statusTypeName = MyStatusContainer->status().type();

	QList<StatusType *> statusTypes = MyStatusContainer->supportedStatusTypes();
	foreach (StatusType *statusType, statusTypes)
	{
		QAction *statusAction = new QAction(MyStatusContainer->statusPixmap(statusType->name()),
											MyStatusContainer->statusNamePrefix() + statusType->displayName(), this);
		statusAction->setCheckable(true);
		statusAction->setData(QVariant::fromValue(statusType));

		ChangeStatusActionGroup->addAction(statusAction);

		if (statusTypeName == statusType->name())
			statusAction->setChecked(true);
	}

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
}

StatusMenu::~StatusMenu()
{
	disconnect(MyStatusContainer, SIGNAL(statusChanged()), this, SLOT(statusChanged()));
}

void StatusMenu::addToMenu(QMenu *menu)
{
	if (0 == ChangeStatusActionGroup->actions().count())
		return;

	StatusType *statusType = ChangeStatusActionGroup->actions()[0]->data().value<StatusType *>();
	if (0 == statusType)
		return;

	StatusGroup *currentGroup = statusType->statusGroup();
	bool setDescriptionAdded = false;
	bool ignoreSeparator = false;

	foreach (QAction *action, ChangeStatusActionGroup->actions())
	{
		StatusType *statusType = action->data().value<StatusType *>();
		if (0 == statusType)
			return;

		if (!setDescriptionAdded && statusType->statusGroup() &&
				statusType->statusGroup()->sortIndex() >= StatusGroup::StatusGroupSortIndexAfterSetDescription)
		{
			menu->addSeparator();
			menu->addAction(ChangeDescription);
		}

		if (statusType->statusGroup() != currentGroup)
		{
			menu->addSeparator();
			currentGroup = statusType->statusGroup();
		}

		menu->addAction(action);
	}

	menu->addSeparator();
	menu->addAction(ChangePrivateStatus);

	connect(menu, SIGNAL(aboutToHide()), this, SLOT(aboutToHide()));
}

void StatusMenu::aboutToHide()
{
	QMenu *menu = dynamic_cast<QMenu *>(sender());
	if (menu)
		MousePositionBeforeMenuHide = menu->pos();
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
	if (!AccountManager::instance()->defaultAccount().isNull())
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
