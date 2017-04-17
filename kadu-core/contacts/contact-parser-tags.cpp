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

#include "contact-parser-tags.h"
#include "contact-parser-tags.moc"

#include "accounts/account.h"
#include "icons/icons-manager.h"
#include "icons/kadu-icon.h"
#include "misc/paths-provider.h"
#include "parser/parser.h"
#include "protocols/protocol.h"
#include "status/status-container-manager.h"
#include "status/status-type-manager.h"
#include "talkable/talkable-converter.h"

ContactParserTags::ContactParserTags(QObject *parent) : QObject{parent}
{
}

ContactParserTags::~ContactParserTags()
{
}

void ContactParserTags::setIconsManager(IconsManager *iconsManager)
{
    m_iconsManager = iconsManager;
}

void ContactParserTags::setParser(Parser *parser)
{
    m_parser = parser;
}

void ContactParserTags::setStatusContainerManager(StatusContainerManager *statusContainerManager)
{
    m_statusContainerManager = statusContainerManager;
}

void ContactParserTags::setStatusTypeManager(StatusTypeManager *statusTypeManager)
{
    m_statusTypeManager = statusTypeManager;
}

void ContactParserTags::setTalkableConverter(TalkableConverter *talkableConverter)
{
    m_talkableConverter = talkableConverter;
}

void ContactParserTags::init()
{
    m_parser->registerTag("avatarPath", [this](Talkable talkable) {
        auto avatarPath = m_talkableConverter->toAvatarPath(talkable);
        if (avatarPath.isEmpty())
            return QString{};
        else
            return PathsProvider::webKitPath(avatarPath);
    });
    m_parser->registerTag("statusIconPath", [this](Talkable talkable) {
        if (m_talkableConverter->toBuddy(talkable).isBlocked())
            return PathsProvider::webKitPath(m_iconsManager->iconPath(KaduIcon{"kadu_icons/blocked", "16x16"}));

        if (m_talkableConverter->toContact(talkable).isBlocking())
            return PathsProvider::webKitPath(m_iconsManager->iconPath(KaduIcon{"kadu_icons/blocking", "16x16"}));

        auto status = m_talkableConverter->toStatus(talkable);
        auto account = m_talkableConverter->toAccount(talkable);
        if (auto protocol = account.protocolHandler())
            return PathsProvider::webKitPath(
                m_iconsManager->iconPath(m_statusTypeManager->statusIcon(protocol->statusPixmapPath(), status)));
        else
            return PathsProvider::webKitPath(
                m_iconsManager->iconPath(m_statusContainerManager->statusIcon(Status{status.type()})));
    });
}

void ContactParserTags::done()
{
    m_parser->unregisterTag("avatarPath");
    m_parser->unregisterTag("statusIconPath");
}
