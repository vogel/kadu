/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qcursor.h>
#include <qobject.h>
#include <QContextMenuEvent>
#include <QMoveEvent>
#include <QDropEvent>
#include <QList>
#include <Q3PopupMenu>
#include <QDragEnterEvent>
#include <QShortcut>

#include "action.h"
#include "config_file.h"
#include "debug.h"
#include "icons_manager.h"
#include "message_box.h"
#include "misc.h"
#include "toolbar.h"
#include "kadu.h"

QMap< QString, QList<ToolBar::ToolBarAction> > ToolBar::DefaultActions;

ToolBar::ToolBar(QWidget * parent)
	: QToolBar(parent)/*, dragButton(NULL)*/
{
	kdebugf();

	setAcceptDrops(true);
// 	setMovingEnabled(!DockArea::blocked());

	connect(&KaduActions, SIGNAL(actionLoaded(const QString &)), this, SLOT(actionLoaded(const QString &)));
	connect(&KaduActions, SIGNAL(actionUnloaded(const QString &)), this, SLOT(actionUnloaded(const QString &)));

	kdebugf2();
}


ToolBar::~ToolBar()
{
	kdebugf();

	disconnect(&KaduActions, SIGNAL(actionLoaded(const QString &)), this, SLOT(actionLoaded(const QString &)));
	disconnect(&KaduActions, SIGNAL(actionUnloaded(const QString &)), this, SLOT(actionUnloaded(const QString &)));

	kdebugf2();
}

void ToolBar::addAction(const QString &actionName, bool showLabel, QAction *after)
{
	if (hasAction(actionName))
		return;

	ToolBarAction newAction;
	newAction.actionName = actionName;
	newAction.action = 0;
	newAction.button = 0;
	newAction.showLabel = showLabel;

	if (after)
	{
		FOREACH(i, ToolBarActions)
			if ((*i).action == after)
			{
				ToolBarActions.insert(i, newAction);
				return;
			}
	}

	ToolBarActions.append(newAction);
}

void ToolBar::usersChanged()
{
// 	FOREACH(actionIterator, ToolBarActions)
// 		if ((*actionIterator).button)
// 			(*actionIterator).button->usersChanged();
}

// TODO: optimize
// void ToolBar::moveAction(const QString &actionName, ToolButton *button)
// {
// 	bool actionFirst;
// 	bool showLabel;
// 
// 	FOREACH(actionIterator, ToolBarActions)
// 		if ((*actionIterator).actionName == actionName)
// 		{
// 			if ((*actionIterator).button == button)
// 				return;
// 			actionFirst = true;
// 			break;
// 		}
// 		else if ((*actionIterator).button == button)
// 		{
// 			if ((*actionIterator).actionName == actionName)
// 				return;
// 			actionFirst = false;
// 			break;
// 		}
// 
// 	FOREACH(actionIterator, ToolBarActions)
// 		if ((*actionIterator).actionName == actionName)
// 		{
// 			showLabel = (*actionIterator).showLabel;
// 			delete (*actionIterator).button;
// 			ToolBarActions.remove(actionIterator);
// 			break;
// 		}
// 
// 	addAction(actionName, showLabel, button, !actionFirst);
// 	updateButtons();
// }

void ToolBar::addButtonClicked(QAction *action)
{
//	kdebugmf(KDEBUG_FUNCTION_START | KDEBUG_INFO, "action_index = %d\n", action_index);

	addAction(action->data().toString(), false);
	updateButtons();

	kdebugf2();
}

void ToolBar::moveEvent(QMoveEvent *e)
{
// 	if (offset() != 10000)
// 	{
// 		QWidget *parent = parentWidget();
// 		if (parent && e->pos().x() != 0 && parent->width() == e->pos().x() + width() + 1)
// 			setOffset(10000);
// 	}

	QToolBar::moveEvent(e);
}

void ToolBar::dragEnterEvent(QDragEnterEvent* event)
{
	kdebugf();/*
	ToolBar* source = dynamic_cast<ToolBar*>(event->source());
	if (source)
	{
		QString actionName;
		bool showLabel;

		if (ActionDrag::decode(event, actionName, showLabel))
			event->accept((event->source() == this) || (!hasAction(actionName) && dockArea()->supportsAction(KaduActions[actionName]->actionType())));
		else
			event->accept(false);
	}
	else*/
		event->accept(false);
	kdebugf2();
}

void ToolBar::dropEvent(QDropEvent* event)
{
	// TODO: update ToolbarActions
/*
	kdebugf();
	ToolBar* source = dynamic_cast<ToolBar*>(event->source());

	if (!source)
	{
		event->accept(false);
		return;
	}


	QString actionName;
	bool showLabel;
	if (!ActionDrag::decode(event, actionName, showLabel))
	{
		event->accept(false);
		return;
	}

	ToolBar* source_toolbar = (ToolBar*)event->source();
	ToolButton *child = dynamic_cast<ToolButton *>(childAt(event->pos()));

	if (source_toolbar != this)
	{
		source_toolbar->removeAction(actionName);

		if (child)
			addAction(actionName, showLabel, child);
		else
			addAction(actionName, showLabel);
	}
	else
		moveAction(actionName, child);

	updateButtons();

	event->accept(true);*/

	kdebugf2();
}

void ToolBar::contextMenuEvent(QContextMenuEvent *e)
{
	kdebugf();

// 	if (DockArea::blocked())
// 	{
// 		e->ignore();
// 		return;
// 	}

	QMenu *menu = createContextMenu(dynamic_cast<QToolButton *>(childAt(e->pos())));
	menu->popup(e->globalPos());

// TODO: add something intelligent here
// 	delete menu;

	e->accept();
	kdebugf2();
}

void ToolBar::writeToConfig(QDomElement parent_element)
{
	kdebugf();
	QDomElement toolbar_elem = xml_config_file->createElement(parent_element, "ToolBar");
// 	toolbar_elem.setAttribute("offset", offset());

	FOREACH(toolBarAction, ToolBarActions)
	{
		if ((*toolBarAction).button)
			(*toolBarAction).showLabel = (*toolBarAction).button->toolButtonStyle() != Qt::ToolButtonIconOnly;

		QDomElement button_elem = xml_config_file->createElement(toolbar_elem, "ToolButton");
		button_elem.setAttribute("action_name", (*toolBarAction).actionName);
		button_elem.setAttribute("uses_text_label", (*toolBarAction).showLabel);
	}

	kdebugf2();
}

bool ToolBar::hasAction(const QString &action_name)
{
	kdebugf();

	CONST_FOREACH(toolBarAction, ToolBarActions)
		if ((*toolBarAction).actionName == action_name)
			return true;
	return false;

	kdebugf2();
}

void ToolBar::actionLoaded(const QString &actionName)
{
	if (!hasAction(actionName))
		return;

	QTimer::singleShot(0, this, SLOT(updateButtons()));
}

void ToolBar::actionUnloaded(const QString &actionName)
{
	if (!hasAction(actionName))
		return;

	updateButtons();
}

void ToolBar::updateButtons()
{
	QAction *lastAction = 0;
	ActionWindow *actionWindow = dynamic_cast<ActionWindow *>(parent());

	if (!actionWindow)
		return;

	FOREACH(toolBarAction, ToolBarActions)
	{
		const QString &actionName = (*toolBarAction).actionName;

		if ((*toolBarAction).action)
		{
			if (KaduActions.contains(actionName))
			{
				lastAction = (*toolBarAction).action;
				continue;
			}
			else
			{
				if ((*toolBarAction).action)
				{
					delete (*toolBarAction).action;
					(*toolBarAction).action = 0;
				}
			}
		}

		if (KaduActions.contains(actionName) && actionWindow->supportsActionType(KaduActions[actionName]->type()))
		{
			(*toolBarAction).action = KaduActions.getAction(actionName, dynamic_cast<QWidget *>(parent()));
			QToolBar::addAction((*toolBarAction).action);
			(*toolBarAction).button = dynamic_cast<QToolButton *>(widgetForAction((*toolBarAction).action));

			if ((*toolBarAction).showLabel)
				(*toolBarAction).button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

			lastAction = (*toolBarAction).action;
		}
	}
}

void ToolBar::loadFromConfig(QDomElement toolbar_element)
{
	kdebugf();

// 	setOffset(toolbar_element.attribute("offset").toInt());

	ToolBarActions.clear();

	for (QDomNode n = toolbar_element.firstChild(); !n.isNull(); n = n.nextSibling())
	{
		const QDomElement &button_elem = n.toElement();
		if (button_elem.isNull())
			continue;
		if (button_elem.tagName() != "ToolButton")
			continue;

		ToolBarAction action;

		action.actionName = button_elem.attribute("action_name");
		if (button_elem.attribute("uses_text_label").isNull())
			action.showLabel = false;
		else
			action.showLabel = button_elem.attribute("uses_text_label") == "1";
		action.action = 0;
		action.button = 0;

		ToolBarActions.append(action);
	}

	updateButtons();

	kdebugf2();
}

void ToolBar::addDefaultAction(const QString &toolbar, const QString &actionName, int index, bool showLabel)
{
	kdebugf();

	QList<ToolBarAction> &actions = DefaultActions[toolbar];

	ToolBarAction action;
	action.actionName = actionName;
	action.showLabel = showLabel;
	action.action = 0;
	action.button = 0;

	if (index >= (int)actions.size())
	{
		kdebugm(KDEBUG_ERROR, "requested action index (%d) >= actions size (%u)!\n", index, actions.size());
		printBacktrace("requested action index >= actions size!");
		index = -1;
	}

	if (index < 0)
		actions.push_back(action);
	else
		actions.insert(index, action);

	kdebugf2();
}

void ToolBar::loadDefault()
{
// 	kdebugf();
// 
// 	if (DefaultActions.contains(name()))
// 	{
// 		const QList<ToolBarAction>& actions = DefaultActions[name()];
// 		CONST_FOREACH(i, actions)
// 			addAction((*i).actionName, (*i).showLabel, 0);
// 	}
// 
// 	// TODO: why calling updateButtons does not work?
// 	QTimer::singleShot(0, this, SLOT(updateButtons()));
// 
// 	kdebugf2();
}

const UserGroup* ToolBar::selectedUsers() const
{
// 	kdebugf();
// 	DockArea *dockArea = dynamic_cast<DockArea*>(area());
// 	const UserGroup *users = NULL;

	// dont segfault on floating toolbars
// 	if (dockArea)
// 		users = dockArea->selectedUsers();
// 
// 	kdebugf2();
	return 0; //users;
}

QMenu * ToolBar::createContextMenu(QToolButton *button)
{
	QMenu *menu = new QMenu(this);

	currentButton = button;
	if (button)
	{
		QAction *showLabel = menu->addAction(tr("Show text label"), this, SLOT(showTextLabel()));
		showLabel->setCheckable(true);
		showLabel->setChecked(button->toolButtonStyle() != Qt::ToolButtonIconOnly);

		menu->addAction(tr("Delete button"), this, SLOT(deleteButton()));

		menu->addSeparator();
	}

	QMenu *actionsMenu = new QMenu(tr("Add new button"), this);
	CONST_FOREACH(actionDescription, KaduActions)
	{
		bool supportsAction;
		ActionWindow *actionWindow = 0;
		if (parent())
			actionWindow = dynamic_cast<ActionWindow *>(parent());

		if (actionWindow)
			supportsAction = actionWindow->supportsActionType((*actionDescription)->type());
		else
			supportsAction = (*actionDescription)->type() == ActionDescription::TypeGlobal;

		if (!supportsAction)
			continue;

		if (!hasAction((*actionDescription)->name()))
		{
			QAction *action = actionsMenu->addAction(icons_manager->loadIcon((*actionDescription)->iconName()), (*actionDescription)->text());
			action->setData((*actionDescription)->name());
		}
	}

	if (actionsMenu->isEmpty())
		actionsMenu->addAction(tr("No items to add found"))->setEnabled(false);
	else
		connect(actionsMenu, SIGNAL(triggered(QAction *)), this, SLOT(addButtonClicked(QAction *)));

	menu->addAction(tr("Delete toolbar"), this, SLOT(deleteToolbar()));
	menu->addMenu(actionsMenu);

	QMainWindow *mainWindow = dynamic_cast<QMainWindow *>(parent());

	menu->addSeparator();
	switch (mainWindow->toolBarArea(this))
	{
		case Qt::NoToolBarArea:
		case Qt::TopToolBarArea:
			menu->addAction(tr("Create new toolbar"), parent(), SLOT(addTopToolbar()));
			break;
		case Qt::BottomToolBarArea:
			menu->addAction(tr("Create new toolbar"), parent(), SLOT(addBottomToolbar()));
			break;
		case Qt::LeftToolBarArea:
			menu->addAction(tr("Create new toolbar"), parent(), SLOT(addLeftToolbar()));
			break;
		case Qt::RightToolBarArea:
			menu->addAction(tr("Create new toolbar"), parent(), SLOT(addRightToolbar()));
			break;
	}

	return menu;
}

void ToolBar::deleteToolbar()
{
	kdebugf();
	if (MessageBox::ask(tr("Remove toolbar?"), "Warning", this))
		deleteLater();
	kdebugf2();
}

void ToolBar::showTextLabel()
{
	if (!currentButton)
		return;

	if (currentButton->toolButtonStyle() == Qt::ToolButtonIconOnly)
		currentButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	else
		currentButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
}

void ToolBar::deleteButton()
{
	if (!currentButton)
		return;

	FOREACH(actionIterator, ToolBarActions)
		if ((*actionIterator).button == currentButton)
		{
			// TODO: again, lame solution
			removeAction(currentButton->defaultAction());
			currentButton = 0;

			ToolBarActions.remove(actionIterator);
			return;
		}
}
