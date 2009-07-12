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
#include "status/status-type.h"

#include "status_changer.h"

#include "status-menu.h"

StatusMenu::StatusMenu(StatusContainer *statusContainer, QWidget *parent) :
		QObject(parent), MyStatusContainer(statusContainer)
{
	ChangeStatusActionGroup = new QActionGroup(this);
	ChangeStatusActionGroup->setExclusive(false); // HACK

	// TODO: 0.6.6

	QList<StatusType *> statusTypes = statusContainer->supportedStatusTypes();
	foreach (StatusType *statusType, statusTypes)
	{
		QAction *statusAction = new QAction(statusType->displayName(), this);
		statusAction->setCheckable(true);
		statusAction->setData(QVariant::fromValue(statusType));

		ChangeStatusActionGroup->addAction(statusAction);
		ChangeStatusActions.append(statusAction);
	}
/*
	ChangeStatusToOnline = new QAction(/*icons_manager->loadIcon(s.pixmapName(Online, false, false)), * /tr("Online"), this);
	ChangeStatusToOnline->setCheckable(true);
	ChangeStatusToOnline->setData(0);
	connect(ChangeStatusToOnline, SIGNAL(triggered(bool)), this, SLOT(changeStatus()));

	ChangeStatusToOnlineDesc = new QAction(/*icons_manager->loadIcon(s.pixmapName(Online, true, false)), * /tr("Online (d.)"), this);
	ChangeStatusToOnlineDesc->setCheckable(true);
	ChangeStatusToOnlineDesc->setData(1);
	connect(ChangeStatusToOnlineDesc, SIGNAL(triggered(bool)), this, SLOT(changeStatus()));

	ChangeStatusToBusy = new QAction(/*icons_manager->loadIcon(s.pixmapName(Busy, false, false)), * /tr("Busy"), this);
	ChangeStatusToBusy->setCheckable(true);
	ChangeStatusToBusy->setData(2);
	connect(ChangeStatusToBusy, SIGNAL(triggered(bool)), this, SLOT(changeStatus()));

	ChangeStatusToBusyDesc = new QAction(/*icons_manager->loadIcon(s.pixmapName(Busy, true, false)), * /tr("Busy (d.)"), this);
	ChangeStatusToBusyDesc->setCheckable(true);
	ChangeStatusToBusyDesc->setData(3);
	connect(ChangeStatusToBusyDesc, SIGNAL(triggered(bool)), this, SLOT(changeStatus()));

	ChangeStatusToInvisible = new QAction(/*icons_manager->loadIcon(s.pixmapName(Invisible, false, false)), * /tr("Invisible"), this);
	ChangeStatusToInvisible->setCheckable(true);
	ChangeStatusToInvisible->setData(4);
	connect(ChangeStatusToInvisible, SIGNAL(triggered(bool)), this, SLOT(changeStatus()));

	ChangeStatusToInvisibleDesc = new QAction(/*icons_manager->loadIcon(s.pixmapName(Invisible, true, false)), * /tr("Invisible (d.)"), this);
	ChangeStatusToInvisibleDesc->setCheckable(true);
	ChangeStatusToInvisibleDesc->setData(5);
	connect(ChangeStatusToInvisibleDesc, SIGNAL(triggered(bool)), this, SLOT(changeStatus()));

	ChangeStatusToOffline = new QAction(/*icons_manager->loadIcon(s.pixmapName(Offline, false, false)), * /tr("Offline"), this);
	ChangeStatusToOffline->setCheckable(true);
	ChangeStatusToOffline->setData(6);
	connect(ChangeStatusToOffline, SIGNAL(triggered(bool)), this, SLOT(changeStatus()));

	ChangeStatusToOfflineDesc = new QAction(/*icons_manager->loadIcon(s.pixmapName(Offline, true, false)), * /tr("Offline (d.)"), this);
	ChangeStatusToOfflineDesc->setCheckable(true);
	ChangeStatusToOfflineDesc->setData(7);
	connect(ChangeStatusToOfflineDesc, SIGNAL(triggered(bool)), this, SLOT(changeStatus()));*/

	ChangePrivateStatus = new QAction(tr("Private"), this);
	ChangePrivateStatus->setCheckable(true);
	connect(ChangePrivateStatus, SIGNAL(toggled(bool)), this, SLOT(changeStatusPrivate(bool)));

	bool privateStatus = config_file.readBoolEntry("General", "PrivateStatus");
	ChangePrivateStatus->setChecked(privateStatus);
/*
	ChangeStatusActionGroup->addAction(ChangeStatusToOnline);
	ChangeStatusActionGroup->addAction(ChangeStatusToOnlineDesc);
	ChangeStatusActionGroup->addAction(ChangeStatusToBusy);
	ChangeStatusActionGroup->addAction(ChangeStatusToBusyDesc);
	ChangeStatusActionGroup->addAction(ChangeStatusToInvisible);
	ChangeStatusActionGroup->addAction(ChangeStatusToInvisibleDesc);
	ChangeStatusActionGroup->addAction(ChangeStatusToOffline);
	ChangeStatusActionGroup->addAction(ChangeStatusToOfflineDesc);*/

	statusChanged();
	connect(MyStatusContainer, SIGNAL(statusChanged()), this, SLOT(statusChanged()));
}

StatusMenu::~StatusMenu()
{
	disconnect(MyStatusContainer, SIGNAL(statusChanged()), this, SLOT(statusChanged()));
}

void StatusMenu::addToMenu(QMenu *menu)
{
	if (0 == ChangeStatusActions.count())
		return;

	StatusType *statusType = ChangeStatusActions[0]->data().value<StatusType *>();
	if (0 == statusType)
		return;

	StatusGroup *currentGroup = statusType->statusGroup();

	foreach (QAction *action, ChangeStatusActions)
	{
		StatusType *statusType = ChangeStatusActions[0]->data().value<StatusType *>();
		if (0 == statusType)
			return;

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

void StatusMenu::changeStatus()
{
	QAction *action = dynamic_cast<QAction *>(sender());
	if (!action)
		return;

	foreach (QAction *a, ChangeStatusActionGroup->actions())
		a->setChecked(a == action);

	Status status(MyStatusContainer->status());

	switch (action->data().toInt())
	{
		case 0:
			status.setType(Status::Online);
			status.setDescription("");
			MyStatusContainer->setStatus(status);
			break;
		case 1:
			status.setType(Status::Online);
			status.setDescription(status.description());
			ChooseDescription::show(status, MyStatusContainer, MousePositionBeforeMenuHide);
			break;
		case 2:
			status.setType(Status::Busy);
			status.setDescription("");
			MyStatusContainer->setStatus(status);
			break;
		case 3:
			status.setType(Status::Busy);
			status.setDescription(status.description());
			ChooseDescription::show(status, MyStatusContainer, MousePositionBeforeMenuHide);
			break;
		case 4:
			status.setType(Status::Invisible);
			status.setDescription("");
			MyStatusContainer->setStatus(status);
			break;
		case 5:
			status.setType(Status::Invisible);
			status.setDescription(status.description());
			ChooseDescription::show(status, MyStatusContainer, MousePositionBeforeMenuHide);
			break;
		case 6:
			status.setType(Status::Offline);
			status.setDescription("");
			MyStatusContainer->setStatus(status);
			break;
		case 7:
			status.setType(Status::Offline);
			status.setDescription(status.description());
			ChooseDescription::show(status, MyStatusContainer, MousePositionBeforeMenuHide);
			break;
	}
}

void StatusMenu::changeStatusPrivate(bool toggled)
{
	if (AccountManager::instance()->defaultAccount())
		AccountManager::instance()->defaultAccount()->protocol()->setPrivateMode(toggled);

	config_file.writeEntry("General", "PrivateStatus", toggled);
}

void StatusMenu::statusChanged()
{
	int index;

	switch (MyStatusContainer->status().type())
	{
		case Status::Online:
			index = 0;
			break;
		case Status::Busy:
			index = 2;
			break;
		case Status::Invisible:
			index = 4;
			break;
		default:
			index = 6;
	}

	if (!MyStatusContainer->status().description().isEmpty())
		index++;

	foreach (QAction *action, ChangeStatusActionGroup->actions())
		action->setChecked(index == action->data().toInt());

	if (AccountManager::instance()->defaultAccount() && AccountManager::instance()->defaultAccount()->protocol())
	{
		Protocol *protocol = AccountManager::instance()->defaultAccount()->protocol();
		ChangePrivateStatus->setChecked(protocol->privateMode());
	}

// 	ChangeStatusToOfflineDesc->setEnabled(index != 6);
// 	ChangeStatusToOffline->setEnabled(index != 7);
}
