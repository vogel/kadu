/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2011, 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010, 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "core/core.h"
#include "icons/kadu-icon.h"
#include "misc/paths-provider.h"
#include "parser/parser.h"
#include "protocols/protocol.h"
#include "status/status-container-manager.h"
#include "status/status-type-manager.h"
#include "talkable/talkable-converter.h"

#include "contact-parser-tags.h"

static QString getAvatarPath(Talkable talkable)
{
	auto avatar = Core::instance()->talkableConverter()->toAvatar(talkable);
	if (avatar.pixmap().isNull())
		return QString();
	else
		return PathsProvider::webKitPath(avatar.filePath());
}

static QString getStatusIconPath(Talkable talkable)
{
	if (Core::instance()->talkableConverter()->toBuddy(talkable).isBlocked())
		return KaduIcon("kadu_icons/blocked", "16x16").webKitPath();

	if (Core::instance()->talkableConverter()->toContact(talkable).isBlocking())
		return KaduIcon("kadu_icons/blocking", "16x16").webKitPath();

	auto status = Core::instance()->talkableConverter()->toStatus(talkable);
	auto account = Core::instance()->talkableConverter()->toAccount(talkable);

	if (auto protocol = account.protocolHandler())
		return Core::instance()->statusTypeManager()->statusIcon(protocol->statusPixmapPath(), status).webKitPath();
	else
		return Core::instance()->statusContainerManager()->statusIcon(status.type()).webKitPath();
}

void ContactParserTags::registerParserTags(Parser *parser)
{
	parser->registerTag("avatarPath", getAvatarPath);
	parser->registerTag("statusIconPath", getStatusIconPath);
}

void ContactParserTags::unregisterParserTags(Parser *parser)
{
	parser->unregisterTag("avatarPath");
	parser->unregisterTag("statusIconPath");
}
