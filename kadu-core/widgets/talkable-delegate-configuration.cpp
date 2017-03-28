/*
 * %kadu copyright begin%
 * Copyright 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "talkable-delegate-configuration.h"

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "icons/icons-manager.h"
#include "icons/kadu-icon.h"
#include "widgets/talkable-tree-view.h"

TalkableDelegateConfiguration::TalkableDelegateConfiguration(TalkableTreeView *listView, QObject *parent)
        : QObject{parent}, ListView{listView}, AlwaysShowIdentityName{false}, ShowIdentityName{true},
          ShowMessagePixmap{true}, UseConfigurationColors{false}
{
    Q_ASSERT(ListView);
}

TalkableDelegateConfiguration::~TalkableDelegateConfiguration()
{
}

void TalkableDelegateConfiguration::setConfiguration(Configuration *configuration)
{
    m_configuration = configuration;
}

void TalkableDelegateConfiguration::setIconsManager(IconsManager *iconsManager)
{
    m_iconsManager = iconsManager;
}

void TalkableDelegateConfiguration::init()
{
    DefaultAvatarSize = QSize(32, 32);
    MessagePixmap = m_iconsManager->iconByPath(KaduIcon("protocols/common/message")).pixmap(16, 16);

    configurationUpdated();
}

void TalkableDelegateConfiguration::configurationUpdated()
{
    Font = QFont(m_configuration->deprecatedApi()->readFontEntry("Look", "UserboxFont"), ListView);
    BoldFont = Font;
    BoldFont.setBold(true);

    DescriptionFont = Font;
    DescriptionFont.setPointSize(Font.pointSize() - 2);

    AlwaysShowIdentityName =
        m_configuration->deprecatedApi()->readBoolEntry("Look", "TalkableListAlwaysShowIdentityName");
    ShowAvatars = m_configuration->deprecatedApi()->readBoolEntry("Look", "ShowAvatars");
    AvatarBorder = m_configuration->deprecatedApi()->readBoolEntry("Look", "AvatarBorder");
    AvatarGreyOut = m_configuration->deprecatedApi()->readBoolEntry("Look", "AvatarGreyOut");
    AlignTop = m_configuration->deprecatedApi()->readBoolEntry("Look", "AlignUserboxIconsTop");
    ShowBold = m_configuration->deprecatedApi()->readBoolEntry("Look", "ShowBold");
    ShowDescription = m_configuration->deprecatedApi()->readBoolEntry("Look", "ShowDesc");
    ShowMultiLineDescription = m_configuration->deprecatedApi()->readBoolEntry("Look", "ShowMultilineDesc");
    DescriptionColor = m_configuration->deprecatedApi()->readColorEntry("Look", "DescriptionColor");
    FontColor = m_configuration->deprecatedApi()->readColorEntry("Look", "UserboxFgColor");

    ListView->scheduleDelayedItemsLayout();
}

#include "moc_talkable-delegate-configuration.cpp"
