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

#include "mediaplayer-module.h"

#include "actions/mediaplayer-menu-action.h"
#include "actions/next-action.h"
#include "actions/play-action.h"
#include "actions/prev-action.h"
#include "actions/stop-action.h"
#include "actions/toggle-mediaplayer-statuses-action.h"
#include "actions/volume-down-action.h"
#include "actions/volume-up-action.h"
#include "mediaplayer-configuration-ui-handler.h"
#include "mediaplayer-notification-service.h"
#include "mediaplayer-plugin-object.h"
#include "mediaplayer.h"

MediaplayerModule::MediaplayerModule()
{
	add_type<MediaplayerConfigurationUiHandler>();
	add_type<MediaplayerNotificationService>();
	add_type<MediaplayerMenuAction>();
	add_type<MediaplayerPluginObject>();
	add_type<MediaPlayer>();
	add_type<NextAction>();
	add_type<PlayAction>();
	add_type<PrevAction>();
	add_type<StopAction>();
	add_type<ToggleMediaplayerStatusesAction>();
	add_type<VolumeDownAction>();
	add_type<VolumeUpAction>();
}

MediaplayerModule::~MediaplayerModule()
{
}
