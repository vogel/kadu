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
#include "buddies/avatar.h"
#include "buddies/buddy-preferred-manager.h"
#include "misc/path-conversion.h"
#include "parser/parser.h"
#include "icons-manager.h"
#include "protocols/protocol.h"
#include "status/status-type-manager.h"

#include "contact-parser-tags.h"

static QString getAvatarPath(Contact contact)
{
	if (!contact)
		return "";

	// WORKAROUND http://kadu.net/mantis/view.php?id=1606
	// fix by rewriting parser
	Buddy buddy = contact.ownerBuddy();
	Contact preffered = BuddyPreferredManager::instance()->preferredContact(buddy);
	if (preffered == contact && !buddy.buddyAvatar().isEmpty())
		return webKitPath(buddy.buddyAvatar().filePath());
	else
		return webKitPath(contact.contactAvatar().filePath());
}

static QString getStatusIconPath(Contact contact)
{
	if (!contact)
		return "";

	if (contact.ownerBuddy().isBlocked())
		return webKitPath(IconsManager::instance()->iconPath("kadu_icons", "16x16", "kadu-blocked"));

	if (contact.isBlocking())
		return webKitPath(IconsManager::instance()->iconPath("kadu_icons", "16x16", "kadu-blocking"));

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

	return "";
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
