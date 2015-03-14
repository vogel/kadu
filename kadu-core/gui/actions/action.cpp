/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2011, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtWidgets/QMenu>

#include "accounts/account-manager.h"
#include "buddies/buddy-set.h"
#include "buddies/buddy.h"
#include "gui/actions/action-context.h"
#include "gui/actions/action-description.h"
#include "gui/hot-key.h"
#include "icons/icons-manager.h"
#include "icons/kadu-icon.h"

#include "action.h"

Action::Action(ActionDescription *description, ActionContext *context, QObject *parent) :
		QAction(parent), Description(description), Context(context)
{
	setText(Description->Text);

	if (!Description->icon().isNull())
	{
		connect(IconsManager::instance(), SIGNAL(themeChanged()), this, SLOT(updateIcon()));
		setIcon(Description->icon());
	}

	setCheckable(Description->Checkable);

	connect(this, SIGNAL(changed()), this, SLOT(changedSlot()));
	connect(this, SIGNAL(hovered()), this, SLOT(hoveredSlot()));
	connect(this, SIGNAL(triggered(bool)), this, SLOT(triggeredSlot(bool)));

	connect(context, SIGNAL(changed()), this, SLOT(checkState()));
	checkState();
}

Action::~Action()
{
	emit aboutToBeDestroyed(this);

	// we are real owner of this menu
	if (menu())
	{
		delete menu();
		setMenu(0);
	}
}

ActionContext * Action::context()
{
	return Context;
}

void Action::changedSlot()
{
	if (isCheckable() && isChecked() && !isEnabled())
		setChecked(false);

	emit changed(this);
}

void Action::hoveredSlot()
{
	emit hovered(this);
}

void Action::triggeredSlot(bool checked)
{
	emit triggered(this, checked);
}

void Action::checkState()
{
	Description->updateActionState(this);
}

void Action::updateIcon()
{
	setIcon(Description->icon());
}

void Action::setIcon(const KaduIcon &icon)
{
	QAction::setIcon(icon.icon());
}

void disableEmptyContacts(Action *action)
{
	action->setEnabled(!action->context()->contacts().isEmpty());
}

void disableNoChat(Action *action)
{
	action->setEnabled(action->context()->chat() && !action->context()->buddies().isAnyTemporary());
}

#include "moc_action.cpp"
