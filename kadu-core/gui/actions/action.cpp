/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account-manager.h"
#include "contacts/contact.h"
#include "contacts/contact-set.h"
#include "gui/windows/main-window.h"
#include "hot_key.h"
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
			OnIcon = IconsManager::instance()->loadIcon(Description->IconName);
			OffIcon = IconsManager::instance()->loadIcon(Description->IconName + "_off");

			setIcon(OffIcon);
		}
		else
			setIcon(IconsManager::instance()->loadIcon(Description->IconName));
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
		OnIcon = IconsManager::instance()->loadIcon(Description->IconName);
		OffIcon = IconsManager::instance()->loadIcon(Description->IconName + "_off");

		toggledSlot(isChecked());
	}
	else
		setIcon(IconsManager::instance()->loadIcon(Description->IconName));
}

void disableEmptyUles(Action *action)
{
	action->setEnabled(!action->contacts().isEmpty());
}

