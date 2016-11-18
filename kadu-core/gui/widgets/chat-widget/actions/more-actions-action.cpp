/*
 * %kadu copyright begin%
 * Copyright 2016 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "more-actions-action.h"

#include "gui/actions/actions.h"
#include "gui/actions/action.h"
#include "gui/widgets/chat-edit-box.h"
#include "gui/widgets/chat-edit-widget.h"
#include "gui/widgets/toolbar.h"

#include <QtWidgets/QMenu>

MoreActionsAction::MoreActionsAction(QObject *parent) :
		// using C++ initializers breaks Qt's lupdate
		ActionDescription(parent)
{
	setName(QStringLiteral("moreActionsAction"));
	setText(tr("More..."));
	setType(ActionDescription::TypeChat);
}

MoreActionsAction::~MoreActionsAction()
{
}

void MoreActionsAction::setActions(Actions *actions)
{
	m_actions = actions;
}

void MoreActionsAction::actionTriggered(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	Q_UNUSED(toggled)
	auto action = qobject_cast<Action *>(sender);
	if (!action)
		return;

	auto chatEditBox = qobject_cast<ChatEditBox *>(sender->parent());
	if (!chatEditBox)
		return;

	auto chatWidget = chatEditBox->chatWidget();
	if (!chatWidget)
		return;

	auto widgets = sender->associatedWidgets();
	if (widgets.isEmpty())
		return;

	auto widget = widgets.at(widgets.size() - 1);

	auto parent = widget->parentWidget();
	while (nullptr != parent && nullptr == qobject_cast<ToolBar *>(parent))
		parent = parent->parentWidget();
	auto toolbar = qobject_cast<ToolBar *>(parent);

	QMenu menu;
	auto subMenu = make_owned<QMenu>(tr("More"), &menu);

	for (auto const &actionName : m_actions->keys())
	{
		if (toolbar && toolbar->windowHasAction(actionName, false))
			continue;

		auto actionDescription = m_actions->value(actionName);
		if (ActionDescription::TypeChat == actionDescription->type())
			menu.addAction(m_actions->createAction(actionName, chatEditBox->actionContext(), chatEditBox));
		else if (ActionDescription::TypeUser == actionDescription->type())
			subMenu->addAction(m_actions->createAction(actionName, chatEditBox->actionContext(), chatEditBox));
	}

	menu.addSeparator();
	menu.addMenu(subMenu);
	menu.exec(widget->mapToGlobal(QPoint(0, widget->height())));
}

#include "moc_more-actions-action.cpp"
