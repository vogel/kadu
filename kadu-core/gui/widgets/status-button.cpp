/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QMenu>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "gui/widgets/status-menu.h"
#include "protocols/protocol.h"
#include "status/status-container.h"
#include "status_changer.h"

#include "status-button.h"

StatusButton::StatusButton(StatusContainer *statusContainer, QWidget *parent) :
		QPushButton(parent), MyStatusContainer(statusContainer)
{
	createGui();

	statusChanged();
	connect(MyStatusContainer, SIGNAL(statusChanged()), this, SLOT(statusChanged()));
}


void StatusButton::createGui()
{
	MyStatusMenu = new StatusMenu(MyStatusContainer, this);
	QMenu *menu = new QMenu(this);
	MyStatusMenu->addToMenu(menu);

	setMenu(menu);
}

void StatusButton::statusChanged()
{
	setIcon(MyStatusContainer->statusPixmap());
	setText(MyStatusContainer->statusName() + " " + MyStatusContainer->statusContainerName());
}

void StatusButton::configurationUpdated()
{
	setIcon(MyStatusContainer->statusPixmap());
}
