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

#include "chat/chat.h"

#include "otr-trust-level-chat-store.h"

#include "otr-policy-account-store.h"

void OtrTrustLevelAccountStore::storeTrustLevelToChat(const Chat &chat, OtrTrustLevel::Level level)
{
	chat.addProperty("otr:trustLevel", (int)level, CustomProperties::NonStorable);
}

OtrTrustLevel::Level OtrTrustLevelAccountStore::loadTrustLevelFromChat(const Chat &chat)
{
	return (OtrTrustLevel::Level)chat.property("otr:trustLevel", QVariant()).toInt();
}
