/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtWidgets/QAction>
#include <QtWidgets/QActionGroup>
#include <QtWidgets/QMenu>

#include "core/core.h"
#include "gui/window-manager.h"
#include "gui/windows/status-window.h"
#include "protocols/protocol.h"
#include "status/status-actions.h"
#include "status/status-container.h"
#include "status/status-setter.h"
#include "status/status-type-manager.h"
#include "status/status-type.h"

#include "status-menu.h"

StatusMenu::StatusMenu(StatusContainer *statusContainer, bool includePrefix, QMenu *menu) :
		QObject(menu), Menu(menu), Container(statusContainer)
{
	Actions = new StatusActions(statusContainer, includePrefix, this);

	connect(Actions, SIGNAL(statusActionsRecreated()), this, SLOT(addStatusActions()));
	connect(Actions, SIGNAL(statusActionsRecreated()), this, SIGNAL(menuRecreated()));
	connect(Actions, SIGNAL(statusActionTriggered(QAction *)), this, SLOT(changeStatus(QAction *)));
	connect(Actions, SIGNAL(changeDescriptionActionTriggered(bool)), this, SLOT(changeDescription()));

	connect(Menu, SIGNAL(aboutToHide()), this, SLOT(aboutToHide()));

	addStatusActions();
}

StatusMenu::~StatusMenu()
{
}

void StatusMenu::addStatusActions()
{
	foreach (QAction *action, Actions->actions())
		Menu->addAction(action);
}

void StatusMenu::aboutToHide()
{
	MousePositionBeforeMenuHide = Menu->pos();
}

void StatusMenu::changeStatus(QAction *action)
{
	StatusType statusType = action->data().value<StatusType>();

	for (auto &&container : Container->subStatusContainers())
	{
		Status status(StatusSetter::instance()->manuallySetStatus(container));
		status.setType(statusType);

		StatusSetter::instance()->setStatusManually(container, status);
		container->storeStatus(status);
	}
}

void StatusMenu::changeDescription()
{
	QWidget *statusWindow = StatusWindow::showDialog(Container, Menu);
	WindowManager::instance()->moveToPosition(statusWindow, MousePositionBeforeMenuHide);
}

#include "moc_status-menu.cpp"
