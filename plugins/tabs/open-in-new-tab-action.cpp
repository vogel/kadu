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

#include "open-in-new-tab-action.h"

#include "buddies/buddy-set.h"
#include "chat/chat.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "actions/action-context.h"
#include "actions/action.h"
#include "gui/widgets/chat-widget/chat-widget-manager.h"
#include "gui/widgets/chat-widget/chat-widget-repository.h"
#include "gui/windows/main-configuration-window-service.h"

#include <QtCore/QCoreApplication>

OpenInNewTabAction::OpenInNewTabAction(QObject *parent) :
		// using C++ initializers breaks Qt's lupdate
		ActionDescription(parent)
{
	setIcon(KaduIcon{"internet-group-chat"});
	setName(QStringLiteral("openInNewTabAction"));
	setText(tr("Chat in New Tab"));
	setType(ActionDescription::TypeUser);
}

OpenInNewTabAction::~OpenInNewTabAction()
{
}

void OpenInNewTabAction::setChatWidgetManager(ChatWidgetManager *chatWidgetManager)
{
	m_chatWidgetManager = chatWidgetManager;
}

void OpenInNewTabAction::setChatWidgetRepository(ChatWidgetRepository *chatWidgetRepository)
{
	m_chatWidgetRepository = chatWidgetRepository;
}

void OpenInNewTabAction::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void OpenInNewTabAction::actionTriggered(QAction *sender, bool)
{
	auto action = qobject_cast<Action *>(sender);
	if (!action)
		return;

	auto chat = action->context()->chat();
	if (!chat)
		return;

	auto chatWidget = m_chatWidgetRepository->widgetForChat(chat);
	if (!chatWidget)
	{
		if (m_configuration->deprecatedApi()->readBoolEntry("Chat", "DefaultTabs"))
		{
			chat.addProperty("tabs:tmp-detached", true, CustomProperties::NonStorable);
		}
		else
		{
			chat.addProperty("tabs:tmp-attached", true, CustomProperties::NonStorable);
		}
	}

	m_chatWidgetManager->openChat(chat, OpenChatActivation::Activate);

	chat.removeProperty("tabs:tmp-attached");
	chat.removeProperty("tabs:tmp-detached");
}

void OpenInNewTabAction::updateActionState(Action *action)
{
	if (action->context()->buddies().isAnyTemporary())
	{
		action->setEnabled(false);
		return;
	}

	action->setEnabled(action->context()->chat());

	if (m_configuration->deprecatedApi()->readBoolEntry("Chat", "DefaultTabs"))
		action->setText(QCoreApplication::translate("TabsManager", "Chat in New Window"));
	else
		action->setText(QCoreApplication::translate("TabsManager", "Chat in New Tab"));
}

#include "moc_open-in-new-tab-action.cpp"
