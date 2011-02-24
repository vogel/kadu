/*
 * %kadu copyright begin%
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QMenu>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "configuration/main-configuration.h"
#include "gui/widgets/status-menu.h"
#include "protocols/protocol.h"
#include "status/status-container.h"

#include "status-button.h"

StatusButton::StatusButton(StatusContainer *statusContainer, QWidget *parent) :
		QPushButton(parent), MyStatusContainer(statusContainer), DisplayStatusName(false)
{
	createGui();

	statusChanged();
	connect(MyStatusContainer, SIGNAL(statusChanged()), this, SLOT(statusChanged()));
}

StatusButton::~StatusButton()
{
}

void StatusButton::createGui()
{
	QMenu *menu = new QMenu(this);
	new StatusMenu(MyStatusContainer, menu);

	setMenu(menu);
}

void StatusButton::update()
{
	setIcon(MyStatusContainer->statusIcon());

	if (DisplayStatusName)
		setText(MyStatusContainer->statusDisplayName());
	else
	{
		if (MainConfiguration::instance()->simpleMode())
			setText(MyStatusContainer->statusContainerName());
		else
			setToolTip(MyStatusContainer->statusContainerName());
	}
}

void StatusButton::statusChanged()
{
	update();
}

void StatusButton::configurationUpdated()
{
	update();
}

void StatusButton::setDisplayStatusName(bool displayStatusName)
{
	if (DisplayStatusName != displayStatusName)
	{
		DisplayStatusName = displayStatusName;
		update();
	}
}
