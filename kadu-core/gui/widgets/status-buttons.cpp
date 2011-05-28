/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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

#include <QtGui/QHBoxLayout>
#include <QtGui/QLayoutItem>

#include "accounts/account-manager.h"
#include "configuration/main-configuration-holder.h"
#include "gui/widgets/status-button.h"
#include "status/status-container-manager.h"

#include "status-buttons.h"

StatusButtons::StatusButtons(QWidget *parent) :
		QWidget(parent), Layout(0), HasStretch(0)
{
	createGui();

	triggerAllStatusContainerRegistered();
}

StatusButtons::~StatusButtons()
{
}

void StatusButtons::createGui()
{
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	Layout = new QHBoxLayout(this);
	setStretch(MainConfigurationHolder::instance()->setStatusMode() == SetStatusPerAccount);
}

void StatusButtons::addButton(StatusButton* button)
{
	if (HasStretch)
		Layout->insertWidget(Layout->count() - 1, button);
	else
		Layout->addWidget(button);
}

void StatusButtons::setStretch(bool stretch)
{
	if (HasStretch == stretch)
		return;

	HasStretch = stretch;

	if (HasStretch)
		Layout->addStretch(200);
	else
		delete Layout->takeAt(Layout->count() - 1);
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
	addButton(button);
	Buttons[statusContainer] = button;

	enableStatusName();
	setStretch(MainConfigurationHolder::instance()->setStatusMode() == SetStatusPerAccount);
}

void StatusButtons::statusContainerUnregistered(StatusContainer *statusContainer)
{
	if (Buttons.contains(statusContainer))
	{
		// cannot delete now, because this will modify ConfigurationAwareObject::objects list
		Buttons[statusContainer]->deleteLater();
		Buttons.remove(statusContainer);

		enableStatusName();
		setStretch(MainConfigurationHolder::instance()->setStatusMode() == SetStatusPerAccount);
	}
}
