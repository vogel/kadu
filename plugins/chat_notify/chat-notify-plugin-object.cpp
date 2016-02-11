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

#include "chat-notify-plugin-object.h"

#include "chat-notifier.h"

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/core.h"
#include "notification/notifier-repository.h"

ChatNotifyPluginObject::ChatNotifyPluginObject(QObject *parent) :
		QObject{parent}
{
}

ChatNotifyPluginObject::~ChatNotifyPluginObject()
{
}

void ChatNotifyPluginObject::setChatNotifier(ChatNotifier *chatNotifier)
{
	m_chatNotifier = chatNotifier;
}

void ChatNotifyPluginObject::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void ChatNotifyPluginObject::setNotifierRepository(NotifierRepository *notifierRepository)
{
	m_notifierRepository = notifierRepository;
}

void ChatNotifyPluginObject::init()
{
	createDefaultConfiguration();
	
	m_notifierRepository->registerNotifier(m_chatNotifier);
}

void ChatNotifyPluginObject::done()
{
	m_notifierRepository->unregisterNotifier(m_chatNotifier);
}

void ChatNotifyPluginObject::createDefaultConfiguration()
{
	m_configuration->deprecatedApi()->addVariable("Notify", "FileTransfer_ChatNotifier", true);
	m_configuration->deprecatedApi()->addVariable("Notify", "FileTransfer/IncomingFile_ChatNotifier", true);
	m_configuration->deprecatedApi()->addVariable("Notify", "StatusChanged_ChatNotifier", true);
	m_configuration->deprecatedApi()->addVariable("Notify", "StatusChanged/ToAway_ChatNotifier", true);
	m_configuration->deprecatedApi()->addVariable("Notify", "StatusChanged/ToDoNotDisturb_ChatNotifier", true);
	m_configuration->deprecatedApi()->addVariable("Notify", "StatusChanged/ToFreeForChat_ChatNotifier", true);
	m_configuration->deprecatedApi()->addVariable("Notify", "StatusChanged/ToNotAvailable_ChatNotifier", true);
	m_configuration->deprecatedApi()->addVariable("Notify", "StatusChanged/ToOffline_ChatNotifier", true);
	m_configuration->deprecatedApi()->addVariable("Notify", "StatusChanged/ToOnline_ChatNotifier", true);
	m_configuration->deprecatedApi()->addVariable("Notify", "OTR_ChatNotifier", true);
}

#include "moc_chat-notify-plugin-object.cpp"
