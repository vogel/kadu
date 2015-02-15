/*
 * %kadu copyright begin%
 * Copyright 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat-window-storage-configurator.h"

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/application.h"
#include "gui/windows/chat-window/chat-window-storage-configuration.h"
#include "gui/windows/chat-window/chat-window-storage.h"

ChatWindowStorageConfigurator::ChatWindowStorageConfigurator()
{
	createDefaultConfiguration();
}

void ChatWindowStorageConfigurator::setChatWindowStorage(ChatWindowStorage *chatWindowStorage)
{
	m_chatWindowStorage = chatWindowStorage;
	configurationUpdated();
}

void ChatWindowStorageConfigurator::configurationUpdated()
{
	if (m_chatWindowStorage)
		m_chatWindowStorage.data()->setConfiguration(loadConfiguration());
}

void ChatWindowStorageConfigurator::createDefaultConfiguration() const
{
	Application::instance()->configuration()->deprecatedApi()->addVariable("Chat", "SaveOpenedWindows", true);
}

ChatWindowStorageConfiguration ChatWindowStorageConfigurator::loadConfiguration() const
{
	auto configuration = ChatWindowStorageConfiguration();
	configuration.setStoreOpenedChatWindows(Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Chat", "SaveOpenedWindows", true));
	return configuration;
}
