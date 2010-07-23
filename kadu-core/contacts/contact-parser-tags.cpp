/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * 
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

#include "buddies/avatar.h"
#include "parser/parser.h"

#include "contact-parser-tags.h"
#include <accounts/account.h>

static QString getAvatarPath(Contact contact)
{
	if (!contact)
		return "";

	return contact.contactAvatar().filePath();
}

static QString getStatusIconPath(Contact contact)
{
	if (!contact)
		return "";

	StatusContainer *statusContainer = contact.contactAccount().statusContainer();
	if (!statusContainer)
		return "";

	return statusContainer->statusIconPath(contact.currentStatus().type());
}

void ContactParserTags::registerParserTags()
{
	Parser::registerTag("avatarPath", getAvatarPath);
	Parser::registerTag("statusIconPath", getStatusIconPath);
}

void ContactParserTags::unregisterParserTags()
{
	Parser::unregisterTag("avatarPath", getAvatarPath);
	Parser::unregisterTag("statusIconPath", getStatusIconPath);
}
