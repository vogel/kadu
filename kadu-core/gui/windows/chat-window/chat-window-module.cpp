/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat-window-module.h"

#include "gui/windows/chat-window/chat-window-factory.h"
#include "gui/windows/chat-window/chat-window-manager.h"
#include "gui/windows/chat-window/chat-window-repository.h"
#include "gui/windows/chat-window/chat-window-storage.h"
#include "gui/windows/chat-window/window-chat-widget-container-handler.h"

ChatWindowModule::ChatWindowModule()
{
	add_type<ChatWindowFactory>();
	add_type<ChatWindowManager>();
	add_type<ChatWindowRepository>();
	add_type<ChatWindowStorage>();
	add_type<WindowChatWidgetContainerHandler>();
}

ChatWindowModule::~ChatWindowModule()
{
}
