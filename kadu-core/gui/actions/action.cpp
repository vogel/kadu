/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
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

#include "accounts/account-manager.h"
#include "buddies/buddy.h"
#include "buddies/buddy-set.h"
#include "gui/actions/action-data-source.h"
#include "gui/actions/action-description.h"
#include "gui/hot-key.h"
#include "icons/icons-manager.h"
#include "icons/kadu-icon.h"
#include "protocols/services/chat-service.h"

#include "action.h"

Action::Action(ActionDescription *description, ActionDataSource *dataSource, QObject *parent) :
		QAction(parent), Description(description), DataSource(dataSource)
{
	Q_ASSERT(0 != description);
	Q_ASSERT(0 != dataSource);

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

	checkState();
}

Action::~Action()
{
	emit aboutToBeDestroyed(this);

	// we are real owner of this menu
	if (menu()) {
		delete menu();
		setMenu(0);
	}
}

Contact Action::contact()
{
	ContactSet contactSet = contacts();
	if (1 != contactSet.count())
		return Contact::null;
	else
		return *contactSet.constBegin();
}

ContactSet Action::contacts()
{
	return DataSource->contacts();
}

Buddy Action::buddy()
{
	BuddySet buddySet = buddies();
	if (1 != buddySet.count())
		return Buddy::null;
	else
		return *buddySet.constBegin();
}

BuddySet Action::buddies()
{
	return DataSource->buddies();
}

Chat Action::chat()
{
	return DataSource->chat();
}

StatusContainer * Action::statusContainer()
{
	return DataSource->statusContainer();
}

ActionDataSource * Action::dataSource()
{
	return DataSource;
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
	action->setEnabled(!action->contacts().isEmpty());
}

void disableNoChat(Action *action)
{
	action->setEnabled(action->chat());
}
