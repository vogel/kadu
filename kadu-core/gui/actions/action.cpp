/*
 * %kadu copyright begin%
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

#include "accounts/account-manager.h"
#include "buddies/buddy.h"
#include "buddies/buddy-set.h"
#include "gui/actions/action-data-source.h"
#include "gui/actions/action-description.h"
#include "gui/hot-key.h"
#include "icons-manager.h"
#include "protocols/services/chat-service.h"

#include "action.h"

Action::Action(ActionDescription *description, ActionDataSource *dataSource, QObject *parent) :
		QAction(parent), Description(description), DataSource(dataSource)
{
	setText(Description->Text);

	if (!Description->iconPath().isEmpty())
	{
		connect(IconsManager::instance(), SIGNAL(themeChanged()), this, SLOT(updateIcon()));
		setIcon(IconsManager::instance()->iconByPath(Description->iconPath()));
	}

	setCheckable(Description->Checkable);

	connect(this, SIGNAL(changed()), this, SLOT(changedSlot()));
	connect(this, SIGNAL(hovered()), this, SLOT(hoveredSlot()));
	connect(this, SIGNAL(triggered(bool)), this, SLOT(triggeredSlot(bool)));

	checkState();
}

Action::~Action()
{
}

Contact Action::contact()
{
	ContactSet contactSet = contacts();
	if (1 != contactSet.count())
		return Contact::null;
	else
		return *contactSet.begin();
}

ContactSet Action::contacts()
{
	if (DataSource)
		return DataSource->contacts();
	else
		return ContactSet();
}

Buddy Action::buddy()
{
	BuddySet buddySet = buddies();
	if (1 != buddySet.count())
		return Buddy::null;
	else
		return *buddySet.begin();
}

BuddySet Action::buddies()
{
	if (DataSource)
		return DataSource->buddies();
	else
		return BuddySet();
}

Chat Action::chat()
{
	if (DataSource)
		return DataSource->chat();
	else
		return Chat::null;
}

StatusContainer * Action::statusContainer()
{
	if (DataSource)
		return DataSource->statusContainer();
	else
		return 0;
}

ActionDataSource * Action::dataSource()
{
	return DataSource;
}

void Action::changedSlot()
{
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
	if (Description->EnableCallback)
		(*Description->EnableCallback)(this);
}

void Action::updateIcon()
{
	setIcon(IconsManager::instance()->iconByPath(Description->iconPath()));
}

void disableEmptyContacts(Action *action)
{
	action->setEnabled(!action->contacts().isEmpty());
}

void disableNoChat(Action *action)
{
  action->setEnabled(action->chat());
}
