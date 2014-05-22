/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "gui/widgets/talkable-tree-view.h"
#include "icons/kadu-icon.h"
#include "kadu-application.h"

#include "talkable-delegate-configuration.h"

TalkableDelegateConfiguration::TalkableDelegateConfiguration(TalkableTreeView *listView) :
		ListView(listView), ShowIdentityName(true), ShowMessagePixmap(true), UseConfigurationColors(false)
{
	Q_ASSERT(ListView);

	DefaultAvatarSize = QSize(32, 32);
	MessagePixmap = KaduIcon("protocols/common/message").icon().pixmap(16, 16);

	configurationUpdated();
}

void TalkableDelegateConfiguration::configurationUpdated()
{
	Font = KaduApplication::instance()->configuration()->deprecatedApi()->readFontEntry("Look", "UserboxFont");
	BoldFont = Font;
	BoldFont.setBold(true);

	DescriptionFont = Font;
	DescriptionFont.setPointSize(Font.pointSize() - 2);

	ShowAvatars = KaduApplication::instance()->configuration()->deprecatedApi()->readBoolEntry("Look", "ShowAvatars");
	AvatarBorder = KaduApplication::instance()->configuration()->deprecatedApi()->readBoolEntry("Look", "AvatarBorder");
	AvatarGreyOut = KaduApplication::instance()->configuration()->deprecatedApi()->readBoolEntry("Look", "AvatarGreyOut");
	AlignTop = KaduApplication::instance()->configuration()->deprecatedApi()->readBoolEntry("Look", "AlignUserboxIconsTop");
	ShowBold = KaduApplication::instance()->configuration()->deprecatedApi()->readBoolEntry("Look", "ShowBold");
	ShowDescription = KaduApplication::instance()->configuration()->deprecatedApi()->readBoolEntry("Look", "ShowDesc");
	ShowMultiLineDescription = KaduApplication::instance()->configuration()->deprecatedApi()->readBoolEntry("Look", "ShowMultilineDesc");
	DescriptionColor = KaduApplication::instance()->configuration()->deprecatedApi()->readColorEntry("Look", "DescriptionColor");
	FontColor = KaduApplication::instance()->configuration()->deprecatedApi()->readColorEntry("Look", "UserboxFgColor");

	ListView->scheduleDelayedItemsLayout();
}
