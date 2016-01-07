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

#include "exec-notify-plugin-object.h"

#include "exec-notifier.h"

#include "notification/notification-manager.h"

ExecNotifyPluginObject::ExecNotifyPluginObject(QObject *parent) :
		QObject{parent}
{
}

ExecNotifyPluginObject::~ExecNotifyPluginObject()
{
}

void ExecNotifyPluginObject::setExecNotifier(ExecNotifier *execNotifier)
{
	m_execNotifier = execNotifier;
}

void ExecNotifyPluginObject::setNotificationManager(NotificationManager *notificationManager)
{
	m_notificationManager = notificationManager;
}

void ExecNotifyPluginObject::init()
{
	m_notificationManager->registerNotifier(m_execNotifier);
}

void ExecNotifyPluginObject::done()
{
	m_notificationManager->unregisterNotifier(m_execNotifier);
}

#include "moc_exec-notify-plugin-object.cpp"
