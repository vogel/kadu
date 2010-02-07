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
#include "gui/hot-key.h"
#include "gui/windows/main-window.h"
#include "icons-manager.h"

#include "action.h"

Action::Action(ActionDescription *description, MainWindow *parent) :
		QAction(parent), Description(description)
{
	setText(Description->Text);

	if (Description->Checkable)
	{
		OnText = Description->CheckedText;
		OffText = Description->Text;
	}

	if (!Description->IconName.isEmpty())
	{
		connect(IconsManager::instance(), SIGNAL(themeChanged()), this, SLOT(updateIcon()));

		if (Description->Checkable)
		{
			OnIcon = IconsManager::instance()->iconByName(Description->IconName);
			OffIcon = IconsManager::instance()->iconByName(Description->IconName + "_off");

			setIcon(OffIcon);
		}
		else
			setIcon(IconsManager::instance()->iconByName(Description->IconName));
	}

	setCheckable(Description->Checkable);

	connect(this, SIGNAL(changed()), this, SLOT(changedSlot()));
	connect(this, SIGNAL(hovered()), this, SLOT(hoveredSlot()));
	connect(this, SIGNAL(toggled(bool)), this, SLOT(toggledSlot(bool)));
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
	MainWindow *kaduMainWindow = dynamic_cast<MainWindow *>(parent());
	if (kaduMainWindow)
		return kaduMainWindow->contacts();
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
	MainWindow *kaduMainWindow = dynamic_cast<MainWindow *>(parent());
	if (kaduMainWindow)
		return kaduMainWindow->buddies();
	else
		return BuddySet();
}

void Action::changedSlot()
{
	emit changed(this);
}

void Action::hoveredSlot()
{
	emit hovered(this);
}

void Action::toggledSlot(bool checked)
{
	if (checked)
	{
		if (!OnText.isEmpty())
			setText(OnText);
		if (!OnIcon.isNull())
			setIcon(OnIcon);
	}
	else
	{
		if (!OffText.isEmpty())
			setText(OffText);
		if (!OffIcon.isNull())
			setIcon(OffIcon);
	}
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
	if (Description->Checkable)
	{
		OnIcon = IconsManager::instance()->iconByName(Description->IconName);
		OffIcon = IconsManager::instance()->iconByName(Description->IconName + "_off");

		toggledSlot(isChecked());
	}
	else
		setIcon(IconsManager::instance()->iconByName(Description->IconName));
}

void disableEmptyContacts(Action *action)
{
	action->setEnabled(!action->contacts().isEmpty());
}
