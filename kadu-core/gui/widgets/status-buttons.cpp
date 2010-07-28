/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include <QtGui/QHBoxLayout>

#include "accounts/account-manager.h"
#include "gui/widgets/status-button.h"
#include "status/status-container-manager.h"

#include "status-buttons.h"

StatusButtons::StatusButtons(QWidget *parent) :
		QWidget(parent), Layout(0)
{
	createGui();

	triggerAllStatusContainerRegistered();

	if (0 == StatusContainerManager::instance()->count())
		statusContainerRegistered(StatusContainerManager::instance());

	connect(AccountManager::instance(), SIGNAL(accountUpdated(Account)), this, SLOT(rebuildGui()));
}

StatusButtons::~StatusButtons()
{
}

void StatusButtons::createGui()
{
	Layout = new QHBoxLayout(this);
	Layout->addStretch(200);
}

void StatusButtons::rebuildGui()
{
	triggerAllStatusContainerUnregistered();
	triggerAllStatusContainerRegistered();
}

void StatusButtons::statusContainerRegistered(StatusContainer *statusContainer)
{
	if (Buttons.contains(statusContainer) || !Layout)
		return;

	if (1 == StatusContainerManager::instance()->count())
		statusContainerUnregistered(StatusContainerManager::instance());

	StatusButton *button = new StatusButton(statusContainer);
	Layout->insertWidget(Layout->count() - 1, button);
	Buttons[statusContainer] = button;
}

void StatusButtons::statusContainerUnregistered(StatusContainer *statusContainer)
{
	if (Buttons.contains(statusContainer))
	{
		// cannot delete now, because this will modify ConfigurationAwareObject::objects list
		Buttons[statusContainer]->deleteLater();
		Buttons.remove(statusContainer);
	}
	if (0 == StatusContainerManager::instance()->count())
		statusContainerRegistered(StatusContainerManager::instance());
}
