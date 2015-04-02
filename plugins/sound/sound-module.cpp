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

#include "sound-module.h"

#include "gui/sound-actions.h"
#include "gui/sound-buddy-configuration-widget-factory.h"
#include "gui/sound-chat-configuration-widget-factory.h"
#include "gui/sound-configuration-ui-handler.h"
#include "notification/sound-notifier.h"
#include "sound-manager.h"
#include "sound-theme-manager.h"

SoundModule::SoundModule()
{
	add_type<SoundActions>();
	add_type<SoundBuddyConfigurationWidgetFactory>();
	add_type<SoundChatConfigurationWidgetFactory>();
	add_type<SoundConfigurationUiHandler>();
	add_type<SoundManager>();
	add_type<SoundNotifier>();
	add_type<SoundThemeManager>();
}

SoundModule::~SoundModule()
{
}
