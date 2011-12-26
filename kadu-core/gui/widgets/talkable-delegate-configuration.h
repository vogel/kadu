/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef TALKABLE_DELEGATE_CONFIGURATION_H
#define TALKABLE_DELEGATE_CONFIGURATION_H

#include <QtGui/QColor>
#include <QtGui/QFont>
#include <QtGui/QPixmap>

#include "configuration/configuration-aware-object.h"

class TalkableTreeView;

class TalkableDelegateConfiguration : private ConfigurationAwareObject
{
	Q_DISABLE_COPY(TalkableDelegateConfiguration)

	TalkableTreeView *ListView;

	QFont Font;
	QFont BoldFont;
	QFont DescriptionFont;

	bool AlignTop;
	bool ShowIdentityName;
	bool ShowBold;
	bool ShowDescription;
	bool ShowMultiLineDescription;
	bool ShowAvatars;
	bool UseConfigurationColors;
	bool AvatarBorder;
	bool AvatarGreyOut;
	QColor DescriptionColor;
	QColor FontColor;

	QSize DefaultAvatarSize;
	QPixmap MessagePixmap;

public:
	explicit TalkableDelegateConfiguration(TalkableTreeView *listView);

	void setShowIdentityName(bool showIdentityName) { ShowIdentityName = showIdentityName; }
	void setUseConfigurationColors(bool useConfigurationColors) { UseConfigurationColors = useConfigurationColors; }

	const QFont & font() const { return Font; }
	const QFont & boldFont() const { return BoldFont; }
	const QFont & descriptionFont() const { return DescriptionFont; }

	bool alignTop() const { return AlignTop; }
	bool showIdentityName() const { return ShowIdentityName; }
	bool showBold() const { return ShowBold; }
	bool showDescription() const { return ShowDescription; }
	bool showMultiLineDescription() const { return ShowMultiLineDescription; }
	bool showAvatars() const { return ShowAvatars; }
	bool useConfigurationColors() const { return UseConfigurationColors; }
	bool avatarBorder() const { return AvatarBorder; }
	bool avatarGreyOut() const { return AvatarGreyOut; }
	const QColor & descriptionColor() const { return DescriptionColor; }
	const QColor & fontColor() const { return FontColor; }

	const QSize & defaultAvatarSize() const { return DefaultAvatarSize; }
	const QPixmap & messagePixmap() const { return MessagePixmap; }

	virtual void configurationUpdated();

};

#endif // TALKABLE_DELEGATE_CONFIGURATION_H
