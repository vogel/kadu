/*
 * %kadu copyright begin%
 * Copyright 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "accounts/account-manager.h"
#include "configuration/main-configuration-holder.h"
#include "gui/widgets/status-button.h"
#include "status/status-container-manager.h"

#include "status-buttons.h"

StatusButtons::StatusButtons(QWidget *parent) :
		QToolBar(parent)//, Layout(0), HasStretch(0)
{
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

	triggerAllStatusContainerRegistered();
}

StatusButtons::~StatusButtons()
{
}

void StatusButtons::enableStatusName()
{
	if (MainConfigurationHolder::instance()->setStatusMode() != SetStatusPerAccount && 1 == Buttons.count())
		Buttons.begin().value()->setDisplayStatusName(true);
}

void StatusButtons::disableStatusName()
{
	if (!Buttons.isEmpty())
		Buttons.begin().value()->setDisplayStatusName(false);
}

void StatusButtons::statusContainerRegistered(StatusContainer *statusContainer)
{
	if (Buttons.contains(statusContainer))
		return;

	disableStatusName();

	StatusButton *button = new StatusButton(statusContainer);
	addWidget(button);
	Buttons[statusContainer] = button;

	enableStatusName();
}

void StatusButtons::statusContainerUnregistered(StatusContainer *statusContainer)
{
	StatusButton *button = Buttons.take(statusContainer);
	if (button)
	{
		// cannot delete now, because this will modify ConfigurationAwareObject::objects list
		button->deleteLater();

		enableStatusName();
	}
}
