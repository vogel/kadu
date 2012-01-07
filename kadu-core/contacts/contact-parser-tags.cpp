/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "accounts/account.h"
#include "avatars/avatar.h"
#include "icons/kadu-icon.h"
#include "misc/path-conversion.h"
#include "parser/parser.h"
#include "protocols/protocol.h"
#include "status/status-container-manager.h"
#include "status/status-type-manager.h"

#include "contact-parser-tags.h"

static QString getAvatarPath(Talkable talkable)
{
	const Avatar &avatar = talkable.avatar();
	return webKitPath(avatar.filePath());
}

static QString getStatusIconPath(Talkable talkable)
{
	if (talkable.isBlocked())
		return KaduIcon("kadu_icons", "16x16", "blocked").webKitPath();

	if (talkable.isBlocking())
		return KaduIcon("kadu_icons", "16x16", "blocking").webKitPath();

	const Status &status = talkable.currentStatus();

	Protocol *protocol = talkable.account().protocolHandler();
	if (protocol)
		return StatusTypeManager::instance()->statusIcon(protocol->statusPixmapPath(), status).webKitPath();
	else
		return StatusContainerManager::instance()->statusIcon(status.type()).webKitPath();
}

void ContactParserTags::registerParserTags()
{
	Parser::registerTag("avatarPath", getAvatarPath);
	Parser::registerTag("statusIconPath", getStatusIconPath);
}

void ContactParserTags::unregisterParserTags()
{
	Parser::unregisterTag("avatarPath");
	Parser::unregisterTag("statusIconPath");
}
