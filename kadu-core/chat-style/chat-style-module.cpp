/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat-style-module.h"

#include "chat-style/chat-style-configuration-ui-handler.h"
#include "chat-style/chat-style-manager.h"
#include "chat-style/engine/adium/adium-style-engine.h"
#include "chat-style/engine/configured-chat-style-renderer-factory-provider.h"
#include "chat-style/engine/kadu/kadu-style-engine.h"

ChatStyleModule::ChatStyleModule()
{
    add_type<AdiumStyleEngine>();
    add_type<ChatStyleConfigurationUiHandler>();
    add_type<ChatStyleManager>();
    add_type<ConfiguredChatStyleRendererFactoryProvider>();
    add_type<KaduStyleEngine>();
}

ChatStyleModule::~ChatStyleModule()
{
}
