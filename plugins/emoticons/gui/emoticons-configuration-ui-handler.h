/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "theme/emoticon-theme-manager.h"

#include "configuration/gui/configuration-ui-handler.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class ConfigListWidget;
class ConfigPathListEdit;
class ConfigurationWidget;
class IconsManager;
class PluginInjectedFactory;
class PathsProvider;

/**
 * @addtogroup Emoticons
 * @{
 */

/**
 * @class EmoticonsConfigurationUiHandler
 * @short Handler of configuration UI for emotcions plugin.
 */
class EmoticonsConfigurationUiHandler : public QObject, public ConfigurationUiHandler
{
    Q_OBJECT

    QPointer<IconsManager> m_iconsManager;
    QPointer<PluginInjectedFactory> m_pluginInjectedFactory;
    QPointer<PathsProvider> m_pathsProvider;

    QScopedPointer<EmoticonThemeManager> ThemeManager;
    QPointer<ConfigurationWidget> Widget;
    QPointer<ConfigListWidget> ThemesList;

private slots:
    INJEQT_SET void setIconsManager(IconsManager *iconsManager);
    INJEQT_SET void setPluginInjectedFactory(PluginInjectedFactory *pluginInjectedFactory);
    INJEQT_SET void setPathsProvider(PathsProvider *pathsProvider);
    INJEQT_INIT void init();

    void updateEmoticonThemes();
    void installEmoticonTheme();

public:
    Q_INVOKABLE explicit EmoticonsConfigurationUiHandler(QObject *parent = nullptr);
    virtual ~EmoticonsConfigurationUiHandler();

    virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow) override;
    virtual void mainConfigurationWindowDestroyed() override;
    virtual void mainConfigurationWindowApplied() override;
};

/**
 * @}
 */
