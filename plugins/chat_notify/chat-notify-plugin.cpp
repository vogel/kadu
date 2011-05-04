/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "notify/notification-manager.h"

#include "chat-notifier.h"

#include "chat-notify-plugin.h"

ChatNotifyPlugin::~ChatNotifyPlugin()
{
}

int ChatNotifyPlugin::init(bool firstLoad)
{
	if (firstLoad)
		createDefaultConfiguration();

	NotifierInstance = new ChatNotifier(this);
	NotificationManager::instance()->registerNotifier(NotifierInstance);

	return 0;
}

void ChatNotifyPlugin::done()
{
	NotificationManager::instance()->unregisterNotifier(NotifierInstance);
}

void ChatNotifyPlugin::createDefaultConfiguration()
{
	config_file.addVariable("Notify", "FileTransfer_ChatNotifier", true);
	config_file.addVariable("Notify", "FileTransfer/IncomingFile_ChatNotifier", true);
	config_file.addVariable("Notify", "StatusChanged_ChatNotifier", true);
	config_file.addVariable("Notify", "StatusChanged/ToAway_ChatNotifier", true);
	config_file.addVariable("Notify", "StatusChanged/ToDoNotDisturb_ChatNotifier", true);
	config_file.addVariable("Notify", "StatusChanged/ToFreeForChat_ChatNotifier", true);
	config_file.addVariable("Notify", "StatusChanged/ToNotAvailable_ChatNotifier", true);
	config_file.addVariable("Notify", "StatusChanged/ToOffline_ChatNotifier", true);
	config_file.addVariable("Notify", "StatusChanged/ToOnline_ChatNotifier", true);
}

Q_EXPORT_PLUGIN2(chat_notify, ChatNotifyPlugin)
