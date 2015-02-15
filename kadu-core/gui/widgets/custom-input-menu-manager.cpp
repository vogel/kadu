/*
 * %kadu copyright begin%
 * Copyright 2011 SÅawomir StÄpieÅ (s.stepien@interia.pl)
 * Copyright 2011 Sławomir Stępień (s.stepien@interia.pl)
 * Copyright 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011 Slawomir Stepien (s.stepien@interia.pl)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "gui/actions/action-context-provider.h"
#include "gui/actions/action-context.h"
#include "gui/actions/action-description.h"
#include "gui/actions/action.h"

#include "custom-input-menu-manager.h"

CustomInputMenuManager * CustomInputMenuManager::Instance = 0;

CustomInputMenuManager * CustomInputMenuManager::instance()
{
	if (!Instance)
		Instance = new CustomInputMenuManager();

	return Instance;
}

CustomInputMenuManager::CustomInputMenuManager() :
		InputContextMenuSorted(true)
{
}

void CustomInputMenuManager::sortInputContextMenu()
{
	if (!InputContextMenuSorted)
	{
		qSort(InputContextMenu);
		InputContextMenuSorted = true;
	}
}

void CustomInputMenuManager::addActionDescription(ActionDescription *actionDescription, CustomInputMenuItem::CustomInputMenuCategory category, int priority)
{
	InputContextMenu.append(CustomInputMenuItem(actionDescription, category, priority));
	InputContextMenuSorted = false;
}

void CustomInputMenuManager::removeActionDescription(ActionDescription *actionDescription)
{
	QList<CustomInputMenuItem>::iterator i = InputContextMenu.begin();

	while (i != InputContextMenu.end())
	{
		if ((*i).actionDescription() == actionDescription)
			i = InputContextMenu.erase(i);
		else
			++i;
	}
}

QMenu * CustomInputMenuManager::menu(QWidget *parent)
{
	QMenu *menu = new QMenu(parent);

	QWidget *actionContextWidget = parent;
	ActionContext *actionContext = 0;

	while (actionContextWidget)
	{
		ActionContextProvider *actionContextProvider = dynamic_cast<ActionContextProvider *>(actionContextWidget);
		if (actionContextProvider)
		{
			actionContext = actionContextProvider->actionContext();
			break;
		}
		else
			actionContextWidget = actionContextWidget->parentWidget();
	}

	if (actionContext)
	{
		sortInputContextMenu();
		QList<CustomInputMenuItem>::const_iterator i = InputContextMenu.constBegin();
		CustomInputMenuItem::CustomInputMenuCategory lastCategory = CustomInputMenuItem::MenuCategoryTextEdit;
		bool first = true;

		while (i != InputContextMenu.constEnd())
		{
			if ((!first) && (i->category() != lastCategory))
				menu->addSeparator();

			Action *action = i->actionDescription()->createAction(actionContext, parent);

			if (i->category() == CustomInputMenuItem::MenuCategorySuggestion)
				action->setFont(QFont(QString(), -1, QFont::Bold));

			menu->addAction(action);
			action->checkState();

			lastCategory = i->category();
			first = false;
			++i;
		}

		if (!first)
			menu->addSeparator();
	}

	return menu;
}

#include "moc_custom-input-menu-manager.cpp"
