/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "configuration/configuration-aware-object.h"

#include <QtCore/QPointer>

class ChatWidgetMessageHandler;
class ChatWidgetMessageHandlerConfiguration;

/**
 * @addtogroup Gui
 * @{
 */

/**
 * @class ChatWidgetMessageHandlerConfigurator
 * @short Configurator for ChatWidgetMessageHandler.
 *
 * Sets configuration on ChatWidgetMessageHandler after each configuration
 * file update.
 */
class ChatWidgetMessageHandlerConfigurator : private ConfigurationAwareObject
{

public:
	ChatWidgetMessageHandlerConfigurator();

	void setChatWidgetMessageHandler(ChatWidgetMessageHandler *chatWidgetMessageHandler);
	ChatWidgetMessageHandlerConfiguration loadConfiguration() const;

protected:
	virtual void configurationUpdated() override;

private:
	QPointer<ChatWidgetMessageHandler> m_chatWidgetMessageHandler;

	void createDefaultConfiguration() const;

};

/**
 * @}
 */
