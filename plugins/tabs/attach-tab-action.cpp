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

#include "attach-tab-action.h"

#include "tabs.h"

#include "actions/action.h"
#include "gui/widgets/chat-edit-box.h"
#include "gui/widgets/chat-widget/chat-widget.h"

AttachTabAction::AttachTabAction(QObject *parent) :
		// using C++ initializers breaks Qt's lupdate
		ActionDescription(parent)
{
	setCheckable(true);
	setIcon(KaduIcon{"kadu_icons/tab-detach"});
	setName(QStringLiteral("attachToTabsAction"));
	setText(tr("Attach Chat to Tabs"));
	setType(ActionDescription::TypeChat);
}

AttachTabAction::~AttachTabAction()
{
}

void AttachTabAction::setTabsManager(TabsManager *tabsManager)
{
	m_tabsManager = tabsManager;
}

void AttachTabAction::actionInstanceCreated(Action *action)
{
	auto chatEditBox = qobject_cast<ChatEditBox *>(action->parentWidget());
	if (!chatEditBox)
		return;

	auto chatWidget = chatEditBox->chatWidget();
	if (!chatWidget)
		return;

	action->setChecked(m_tabsManager->tabWidget()->indexOf(chatWidget) != -1);
}

void AttachTabAction::actionTriggered(QAction *sender, bool toggled)
{
	auto chatEditBox = qobject_cast<ChatEditBox *>(sender->parentWidget());
	if (!chatEditBox)
		return;

	auto chatWidget = chatEditBox->chatWidget();
	if (!chatWidget)
		return;

	if (!toggled)
		m_tabsManager->detachChat(chatWidget);
	else
		m_tabsManager->attachChat(chatWidget);
}

#include "moc_attach-tab-action.cpp"
