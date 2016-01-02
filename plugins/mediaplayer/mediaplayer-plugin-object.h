/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "mediaplayer-exports.h"

#include "plugin/plugin-object.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class ConfigurationUiHandlerRepository;
class MediaplayerConfigurationUiHandler;
class MediaPlayer;
class PathsProvider;

class MEDIAPLAYERAPI MediaplayerPluginObject : public PluginObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit MediaplayerPluginObject(QObject *parent = nullptr);
	virtual ~MediaplayerPluginObject();

	virtual void init();
	virtual void done();

	MediaPlayer * mediaPlayer() const;

private:
	QPointer<ConfigurationUiHandlerRepository> m_configurationUiHandlerRepository;
	QPointer<MediaplayerConfigurationUiHandler> m_mediaplayerConfigurationUiHandler;
	QPointer<MediaPlayer> m_mediaPlayer;
	QPointer<PathsProvider> m_pathsProvider;

private slots:
	INJEQT_SETTER void setConfigurationUiHandlerRepository(ConfigurationUiHandlerRepository *configurationUiHandlerRepository);
	INJEQT_SETTER void setMediaplayerConfigurationUiHandler(MediaplayerConfigurationUiHandler *mediaplayerConfigurationUiHandler);
	INJEQT_SETTER void setMediaPlayer(MediaPlayer *mediaPlayer);
	INJEQT_SETTER void setPathsProvider(PathsProvider *pathsProvider);

};
