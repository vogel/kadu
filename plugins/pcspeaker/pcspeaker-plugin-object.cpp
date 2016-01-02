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

#include "pcspeaker-plugin-object.h"

#include "pcspeaker-notifier.h"

#include "notification/notification-manager.h"

PCSpeakerPluginObject::PCSpeakerPluginObject(QObject *parent) :
		PluginObject{parent}
{
}

PCSpeakerPluginObject::~PCSpeakerPluginObject()
{
}

void PCSpeakerPluginObject::setNotificationManager(NotificationManager *notificationManager)
{
	m_notificationManager = notificationManager;
}

void PCSpeakerPluginObject::setPCSpeakerNotifier(PCSpeakerNotifier *pcSpeakerNotifier)
{
	m_pcSpeakerNotifier = pcSpeakerNotifier;
}

void PCSpeakerPluginObject::init()
{
	m_notificationManager->registerNotifier(m_pcSpeakerNotifier);
}

void PCSpeakerPluginObject::done()
{
	m_notificationManager->unregisterNotifier(m_pcSpeakerNotifier);
}

#include "moc_pcspeaker-plugin-object.cpp"
