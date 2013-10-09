/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include <QtGui/QMenu>

#include "gui/actions/action-context.h"
#include "gui/actions/action.h"
#include "gui/status-icon.h"
#include "gui/widgets/status-menu.h"

#include "change-status-action.h"

ChangeStatusAction::ChangeStatusAction(QObject *parent) :
		ActionDescription(parent)
{
	setType(ActionDescription::TypeGlobal);
	setName("openStatusAction");
	setIcon(KaduIcon("kadu_icons/change-status"));
	setText(tr("Change Status"));

	registerAction();
}

ChangeStatusAction::~ChangeStatusAction()
{
}

QMenu * ChangeStatusAction::menuForAction(Action *action)
{
	StatusContainer *container = action->context()->statusContainer();
	if (!container)
		return 0;

	// no parents for menu as it is destroyed manually by Action class
	QMenu *menu = new QMenu();
	new StatusMenu(container, false, menu);
	return menu;
}

void ChangeStatusAction::actionInstanceCreated(Action *action)
{
	ActionDescription::actionInstanceCreated(action);

	StatusContainer *statusContainer = action->context()->statusContainer();
	if (statusContainer)
	{
		StatusIcon *icon = new StatusIcon(statusContainer, action);
		connect(icon, SIGNAL(iconUpdated(KaduIcon)), action, SLOT(setIcon(KaduIcon)));
		action->setIcon(icon->icon());
	}
}

#include "moc_change-status-action.cpp"
