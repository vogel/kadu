/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat-style/engine/chat-style-renderer-factory-provider.h"

/**
 * @addtogroup ChatStyle
 * @{
 */

/**
 * @class ConfiguredChatStyleRendererFactoryProvider
 * @short Implementation of ChatStyleRendererFactoryProvider with setter.
 *
 * This implementation of ChatStyleRendererFactoryProvider is used to provide every needing
 * object with ChatStyleRendererFactory that is configured by user choice. Method
 * setChatStyleRendererFactory() is called every time program configuration regarding
 * chat style is changed.
 */
class ConfiguredChatStyleRendererFactoryProvider : public ChatStyleRendererFactoryProvider
{
	Q_OBJECT

public:
	explicit ConfiguredChatStyleRendererFactoryProvider(QObject *parent = nullptr);
	virtual ~ConfiguredChatStyleRendererFactoryProvider();

	virtual std::shared_ptr<ChatStyleRendererFactory> chatStyleRendererFactory() const override;

	/**
	 * @short Set new instance of ChatStyleRendererFactory to be stored in this provider.
	 * @param chatStyleRendererFactory New instance of ChatStyleRendererFactory.
	 */
	void setChatStyleRendererFactory(std::unique_ptr<ChatStyleRendererFactory> chatStyleRendererFactory);

private:
	std::shared_ptr<ChatStyleRendererFactory> m_chatStyleRendererFactory;

};

/**
 * @}
 */
