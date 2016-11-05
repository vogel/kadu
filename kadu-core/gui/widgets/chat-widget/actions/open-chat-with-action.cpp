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

#include "open-chat-with-action.h"

#include "gui/actions/actions.h"
#include "gui/actions/action.h"
#include "gui/windows/open-chat-with/open-chat-with-service.h"

OpenChatWithAction::OpenChatWithAction(QObject *parent) :
		// using C++ initializers breaks Qt's lupdate
		ActionDescription(parent)
{
	setIcon(KaduIcon{"internet-group-chat"});
	setName(QStringLiteral("openChatWithAction"));
	setText(tr("Open Chat with..."));
	setType(ActionDescription::TypeUser);
}

OpenChatWithAction::~OpenChatWithAction()
{
}

void OpenChatWithAction::setOpenChatWithService(OpenChatWithService *openChatWithService)
{
	m_openChatWithService = openChatWithService;
}

void OpenChatWithAction::init()
{
	registerAction(actionsRegistry());
}

void OpenChatWithAction::actionTriggered(QAction *sender, bool)
{
	auto action = qobject_cast<Action *>(sender);
	if (!action)
		return;

	m_openChatWithService->show();
}

#include "moc_open-chat-with-action.cpp"
