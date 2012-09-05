/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef EMOTICONS_CONFIGURATION_UI_HANDLER_H
#define EMOTICONS_CONFIGURATION_UI_HANDLER_H

#include "theme/emoticon-theme-manager.h"

#include "gui/windows/main-configuration-window.h"

class EmoticonsConfigurationUiHandler : public ConfigurationUiHandler
{
	Q_OBJECT

	QScopedPointer<EmoticonThemeManager> ThemeManager;
	QWeakPointer<ConfigurationWidget> Widget;
	QWeakPointer<ConfigComboBox> EmoticonsStyleComboBox;
	QWeakPointer<ConfigComboBox> EmoticonsThemeComboBox;

private slots:
	void updateEmoticonThemes();
	void emoticonThemeSelected(int index);

public:
	explicit EmoticonsConfigurationUiHandler(QObject *parent = 0);
	virtual ~EmoticonsConfigurationUiHandler();

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

};

#endif // EMOTICONS_CONFIGURATION_UI_HANDLER_H
