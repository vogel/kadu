/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "core/core.h"
#include "gui/windows/choose-description.h"
#include "protocols/protocol.h"
#include "status/status-actions.h"
#include "status/status-group.h"
#include "status/status-type.h"
#include "status/status-type-manager.h"

#include "status-menu.h"

StatusMenu::StatusMenu(StatusContainer *statusContainer, QMenu *menu, bool commonStatusIcons) :
		QObject(menu), Menu(menu), MyStatusContainer(statusContainer)
{
	Actions = new StatusActions(MyStatusContainer, this, commonStatusIcons);

	connect(Actions, SIGNAL(statusActionTriggered(QAction *)), this, SLOT(changeStatus(QAction *)));
	connect(Actions, SIGNAL(changeDescriptionActionTriggered(bool)), this, SLOT(changeDescription()));

// 	connect(MyStatusContainer, SIGNAL(updated()), this, SLOT(statusContainerUpdated()));

	connect(Menu, SIGNAL(aboutToHide()), this, SLOT(aboutToHide()));

	foreach (QAction *action, Actions->actions())
		Menu->addAction(action);
}

StatusMenu::~StatusMenu()
{
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
