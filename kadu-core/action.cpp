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
#include "contacts/contact-list.h"
#include "hot_key.h"
#include "icons_manager.h"
#include "kadu_main_window.h"

#include "action.h"

KaduAction::KaduAction(ActionDescription *description, KaduMainWindow *parent)
	: QAction(parent), Description(description)
{
	setText(Description->Text);

	if (Description->Checkable)
	{
		OnText = Description->CheckedText;
		OffText = Description->Text;
	}

	if (!Description->IconName.isEmpty())
	{
		connect(icons_manager, SIGNAL(themeChanged()), this, SLOT(updateIcon()));

		if (Description->Checkable)
		{
			OnIcon = icons_manager->loadIcon(Description->IconName);
			OffIcon = icons_manager->loadIcon(Description->IconName + "_off");

			setIcon(OffIcon);
		}
		else
			setIcon(icons_manager->loadIcon(Description->IconName));
	}

	setCheckable(Description->Checkable);

	connect(this, SIGNAL(changed()), this, SLOT(changedSlot()));
	connect(this, SIGNAL(hovered()), this, SLOT(hoveredSlot()));
	connect(this, SIGNAL(toggled(bool)), this, SLOT(toggledSlot(bool)));
	connect(this, SIGNAL(triggered(bool)), this, SLOT(triggeredSlot(bool)));

	checkState();
}

KaduAction::~KaduAction()
{
}

Contact KaduAction::contact()
{
	ContactList contactList = contacts();
	if (1 != contactList.count())
		return Contact::null;
	else
		return contactList[0];
}

ContactList KaduAction::contacts()
{
	KaduMainWindow *kaduMainWindow = dynamic_cast<KaduMainWindow *>(parent());
	if (kaduMainWindow)
		return kaduMainWindow->contacts();
	else
		return ContactList();
}

void KaduAction::changedSlot()
{
	emit changed(this);
}

void KaduAction::hoveredSlot()
{
	emit hovered(this);
}

void KaduAction::toggledSlot(bool checked)
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

void KaduAction::triggeredSlot(bool checked)
{
	emit triggered(this, checked);
}

void KaduAction::checkState()
{
	if (Description->EnableCallback)
		(*Description->EnableCallback)(this);
}

void KaduAction::updateIcon()
{
	if (Description->Checkable)
	{
		OnIcon = icons_manager->loadIcon(Description->IconName);
		OffIcon = icons_manager->loadIcon(Description->IconName + "_off");

		toggledSlot(isChecked());
	}
	else
		setIcon(icons_manager->loadIcon(Description->IconName));
}

ActionDescription::ActionDescription(QObject *parent, ActionType Type, const QString &Name, QObject *Object, const char *Slot,
	const QString &IconName, const QString &Text, bool Checkable, const QString &CheckedText, ActionBoolCallback enableCallback)
	: QObject(parent), ShortcutItem(""), ShortcutContext(Qt::WidgetShortcut)
{
	this->Type = Type;
	this->Name = Name;
	this->Object = Object;
	this->Slot = Slot;
	this->IconName = IconName;
	this->Text = Text;
	this->Checkable = Checkable;
	this->CheckedText = CheckedText;
	this->EnableCallback = enableCallback;
	this->deleted = 0;

	KaduActions.insert(this);
}

ActionDescription::~ActionDescription()
{
	deleted = 1;
	qDeleteAll(MappedActions);
	MappedActions.clear();
	KaduActions.remove(this);
}

void ActionDescription::actionDestroyed(QObject *action)
{
	KaduAction *kaduAction = static_cast<KaduAction *>(action);
	if (!kaduAction)
		return;

	KaduMainWindow *kaduMainWindow = static_cast<KaduMainWindow *>(action->parent());
	if (!kaduMainWindow)
		return;

	if (!deleted && MappedActions.contains(kaduMainWindow))
		MappedActions.remove(kaduMainWindow);
}

void ActionDescription::setShortcut(QString configItem, Qt::ShortcutContext context)
{
	ShortcutItem = configItem;
	ShortcutContext = context;

	configurationUpdated();
}

KaduAction * ActionDescription::createAction(KaduMainWindow *kaduMainWindow)
{
	if (MappedActions.contains(kaduMainWindow))
		return MappedActions[kaduMainWindow];

	KaduAction *result = new KaduAction(this, kaduMainWindow);
	MappedActions[kaduMainWindow] = result;

	connect(result, SIGNAL(destroyed(QObject *)), this, SLOT(actionDestroyed(QObject *)));
	connect(result, SIGNAL(triggered(QAction *, bool)), Object, Slot);

	emit actionCreated(result);

	if (ShortcutContext != Qt::ApplicationShortcut)
	{
		result->setShortcut(HotKey::shortCutFromFile("ShortCuts", ShortcutItem));
		result->setShortcutContext(ShortcutContext);
	}
	else if (MappedActions.values().count() == 1)
	{
		result->setShortcut(HotKey::shortCutFromFile("ShortCuts", ShortcutItem));
		result->setShortcutContext(ShortcutContext);
	}
	return result;
}

QList<KaduAction *> ActionDescription::actions()
{
	return MappedActions.values();
}

KaduAction * ActionDescription::action(KaduMainWindow *kaduMainWindow)
{
	if (MappedActions.contains(kaduMainWindow))
		return MappedActions[kaduMainWindow];
	else
		return 0;
}


void ActionDescription::configurationUpdated()
{
	if (ShortcutItem.isEmpty())
		return;

	if (ShortcutContext != Qt::ApplicationShortcut)
	{
		foreach (KaduAction *action, MappedActions.values())
		{
			action->setShortcut(HotKey::shortCutFromFile("ShortCuts", ShortcutItem));
			action->setShortcutContext(ShortcutContext);
		}
	}
	else if (MappedActions.values().count() > 0)
	{
		MappedActions.values()[0]->setShortcut(HotKey::shortCutFromFile("ShortCuts", ShortcutItem));
		MappedActions.values()[0]->setShortcutContext(ShortcutContext);
	}
}

Actions::Actions()
{
}

void Actions::insert(ActionDescription *action)
{
	QMap<QString, ActionDescription *>::insert(action->name(), action);
	emit actionLoaded(action->name());
}

void Actions::remove(ActionDescription *action)
{
	QMap<QString, ActionDescription *>::remove(action->name());

	emit actionUnloaded(action->name());
}

QAction * Actions::createAction(const QString &name, KaduMainWindow *kaduMainWindow)
{
	if (!contains(name))
		return 0;

	KaduAction *result = (*this)[name]->createAction(kaduMainWindow);
	kaduMainWindow->actionAdded(result);

	emit actionCreated(result);

	return result;
}

void disableEmptyUles(KaduAction *action)
{
	action->setEnabled(!action->contacts().isEmpty());
}

Actions KaduActions;
