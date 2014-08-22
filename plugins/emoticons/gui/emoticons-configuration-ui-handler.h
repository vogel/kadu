/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

class ConfigListWidget;
class ConfigPathListEdit;

/**
 * @addtogroup Emoticons
 * @{
 */

/**
 * @class EmoticonsConfigurationUiHandler
 * @short Handler of configuration UI for emotcions plugin.
 */
class EmoticonsConfigurationUiHandler : public ConfigurationUiHandler
{
	Q_OBJECT

	QScopedPointer<EmoticonThemeManager> ThemeManager;
	QPointer<ConfigurationWidget> Widget;
	QPointer<ConfigListWidget> ThemesList;

private slots:
	void updateEmoticonThemes();
	void installEmoticonTheme();

public:
	explicit EmoticonsConfigurationUiHandler(QObject *parent = 0);
	virtual ~EmoticonsConfigurationUiHandler();

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

};

/**
 * @}
 */

#endif // EMOTICONS_CONFIGURATION_UI_HANDLER_H
