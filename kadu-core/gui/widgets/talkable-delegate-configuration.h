/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#pragma once

#include "configuration/configuration-aware-object.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtGui/QColor>
#include <QtGui/QFont>
#include <QtGui/QPixmap>
#include <injeqt/injeqt.h>

class Configuration;
class IconsManager;
class TalkableTreeView;

class TalkableDelegateConfiguration : public QObject, private ConfigurationAwareObject
{
	Q_OBJECT

	QPointer<Configuration> m_configuration;
	QPointer<IconsManager> m_iconsManager;

	TalkableTreeView *ListView;

	QFont Font;
	QFont BoldFont;
	QFont DescriptionFont;

	bool AlignTop;
	bool AlwaysShowIdentityName;
	bool ShowIdentityName;
	bool ShowBold;
	bool ShowDescription;
	bool ShowMultiLineDescription;
	bool ShowAvatars;
	bool ShowMessagePixmap;
	bool UseConfigurationColors;
	bool AvatarBorder;
	bool AvatarGreyOut;
	QColor DescriptionColor;
	QColor FontColor;

	QSize DefaultAvatarSize;
	QPixmap MessagePixmap;

private slots:
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_SET void setIconsManager(IconsManager *iconsManager);
	INJEQT_INIT void init();

public:
	explicit TalkableDelegateConfiguration(TalkableTreeView *listView, QObject *parent = nullptr);
	virtual ~TalkableDelegateConfiguration();

	void setShowIdentityName(bool showIdentityName) { ShowIdentityName = showIdentityName; }
	void setUseConfigurationColors(bool useConfigurationColors) { UseConfigurationColors = useConfigurationColors; }
	void setShowMessagePixmap(bool showMessagePixmap) { ShowMessagePixmap = showMessagePixmap; }

	const QFont & font() const { return Font; }
	const QFont & boldFont() const { return BoldFont; }
	const QFont & descriptionFont() const { return DescriptionFont; }

	bool alignTop() const { return AlignTop; }
	bool alwaysShowIdentityName() const { return AlwaysShowIdentityName; }
	bool showIdentityName() const { return ShowIdentityName; }
	bool showBold() const { return ShowBold; }
	bool showDescription() const { return ShowDescription; }
	bool showMultiLineDescription() const { return ShowMultiLineDescription; }
	bool showAvatars() const { return ShowAvatars; }
	bool showMessagePixmap() const { return ShowMessagePixmap; }
	bool useConfigurationColors() const { return UseConfigurationColors; }
	bool avatarBorder() const { return AvatarBorder; }
	bool avatarGreyOut() const { return AvatarGreyOut; }
	const QColor & descriptionColor() const { return DescriptionColor; }
	const QColor & fontColor() const { return FontColor; }

	const QSize & defaultAvatarSize() const { return DefaultAvatarSize; }
	const QPixmap & messagePixmap() const { return MessagePixmap; }

	virtual void configurationUpdated();

};
