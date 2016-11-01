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

#include "auto-send-action.h"

#include "configuration/deprecated-configuration-api.h"
#include "gui/actions/actions.h"
#include "gui/actions/action.h"
#include "gui/configuration/chat-configuration-holder.h"
#include "gui/widgets/chat-edit-box.h"
#include "gui/widgets/chat-edit-widget.h"
#include "gui/widgets/toolbar.h"
#include "misc/memory.h"

#include <QtWidgets/QMenu>

AutoSendAction::AutoSendAction(QObject *parent) :
		// using C++ initializers breaks Qt's lupdate
		ActionDescription(parent)
{
	setCheckable(true);
	setIcon(KaduIcon{"kadu_icons/enter"});
	setName(QStringLiteral("autoSendAction"));
	setText(tr("More..."));
	setType(ActionDescription::TypeChat);
}

AutoSendAction::~AutoSendAction()
{
}

void AutoSendAction::setChatConfigurationHolder(ChatConfigurationHolder* chatConfigurationHolder)
{
	m_chatConfigurationHolder = chatConfigurationHolder;
}

void AutoSendAction::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void AutoSendAction::init()
{
	registerAction(actionsRegistry());
}

void AutoSendAction::actionInstanceCreated(Action *action)
{
	action->setChecked(m_configuration->deprecatedApi()->readBoolEntry("Chat", "AutoSend"));
	action->setToolTip(tr("Return Sends Message"));
}

void AutoSendAction::actionTriggered(QAction *, bool toggled)
{
	m_configuration->deprecatedApi()->writeEntry("Chat", "AutoSend", toggled);
	m_chatConfigurationHolder->configurationUpdated();
	autoSendActionCheck();
}

void AutoSendAction::configurationUpdated()
{
	ActionDescription::configurationUpdated();
	autoSendActionCheck();
}

void AutoSendAction::autoSendActionCheck()
{
 	auto check = m_configuration->deprecatedApi()->readBoolEntry("Chat", "AutoSend");
 	for (auto const action : actions())
 		action->setChecked(check);
}

#include "moc_auto-send-action.cpp"
