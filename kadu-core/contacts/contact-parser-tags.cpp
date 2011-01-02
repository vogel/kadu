/*
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
#include "buddies/buddy-preferred-manager.h"
#include "misc/path-conversion.h"
#include "parser/parser.h"
#include "icons-manager.h"
#include "protocols/protocol.h"
#include "status/status-type-manager.h"

#include "contact-parser-tags.h"

static QString getAvatarPath(BuddyOrContact buddyOrContact)
{
	Avatar avatar;
	if (BuddyOrContact::ItemBuddy == buddyOrContact.type())
		avatar = buddyOrContact.buddy().buddyAvatar();

	if (!avatar)
		avatar = buddyOrContact.contact().contactAvatar();

	if (!avatar)
		return QString();

	return webKitPath(avatar.filePath());
}

static QString getStatusIconPath(BuddyOrContact buddyOrContact)
{
	Buddy buddy = buddyOrContact.buddy();
	Contact contact = buddyOrContact.contact();

	if (buddy.isBlocked())
		return webKitPath(IconsManager::instance()->iconPath("kadu_icons", "16x16", "blocked"));

	if (contact.isBlocking())
		return webKitPath(IconsManager::instance()->iconPath("kadu_icons", "16x16", "blocking"));

	if (contact.contactAccount())
	{
		Protocol *protocol = contact.contactAccount().protocolHandler();
		if (protocol)
		{
			StatusTypeManager* statustypemanager = StatusTypeManager::instance();
			if (statustypemanager)
			{
				Status status = contact.currentStatus();
				QString iconpath = statustypemanager->statusIconFullPath(protocol->statusPixmapPath(), status.type(), !status.description().isEmpty(), false);
				if (!iconpath.isEmpty())
					return webKitPath(iconpath);
			}
		}
	}

	return QString();
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
