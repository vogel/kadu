/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "configuration/configuration-file.h"
#include "icons-manager.h"

#include "buddies-list-view-delegate-configuration.h"

BuddiesListViewDelegateConfiguration::BuddiesListViewDelegateConfiguration() :
		ShowAccountName(true)
{
	DefaultAvatarSize = IconsManager::instance()->pixmapByPath("32x32/system-users.png").size();
	MessagePixmap = IconsManager::instance()->pixmapByPath("protocols/common/16x16/message.png");

	configurationUpdated();
}

void BuddiesListViewDelegateConfiguration::configurationUpdated()
{
	Font = config_file.readFontEntry("Look", "UserboxFont");
	BoldFont = Font;
	BoldFont.setBold(true);

	ShowAccountName = !config_file.readBoolEntry("General", "SimpleMode", true);

	DescriptionFont = Font;
	DescriptionFont.setPointSize(Font.pointSize() - 2);

	ShowAvatars = config_file.readBoolEntry("Look", "ShowAvatars");
	AvatarBorder = config_file.readBoolEntry("Look", "AvatarBorder");
	AvatarGreyOut = config_file.readBoolEntry("Look", "AvatarGreyOut");
	AlignTop = config_file.readBoolEntry("Look", "AlignUserboxIconsTop");
	ShowBold = config_file.readBoolEntry("Look", "ShowBold");
	ShowDescription = config_file.readBoolEntry("Look", "ShowDesc");
	ShowMultiLineDescription = config_file.readBoolEntry("Look", "ShowMultilineDesc");
	DescriptionColor = config_file.readColorEntry("Look", "DescriptionColor");
}
