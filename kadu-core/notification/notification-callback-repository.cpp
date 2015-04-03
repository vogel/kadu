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

#include "notification-callback-repository.h"

#include "notification/notification-callback.h"

NotificationCallbackRepository::NotificationCallbackRepository(QObject *parent) :
		QObject{parent}
{
}

NotificationCallbackRepository::~NotificationCallbackRepository()
{
}

void NotificationCallbackRepository::addCallback(NotificationCallback callback)
{
	auto it = std::find(std::begin(m_callbacks), std::end(m_callbacks), callback);
	if (it == std::end(m_callbacks))
		m_callbacks.push_back(callback);
}

void NotificationCallbackRepository::removeCallback(NotificationCallback callback)
{
	auto it = std::find(std::begin(m_callbacks), std::end(m_callbacks), callback);
	if (it != std::end(m_callbacks))
		m_callbacks.erase(it);
}

NotificationCallback NotificationCallbackRepository::callback(const QString &name)
{
	auto it = std::find_if(std::begin(m_callbacks), std::end(m_callbacks), [&name](const NotificationCallback &x){
		return x.name() == name;
	});
	if (it != std::end(m_callbacks))
		return *it;
	else
		return NotificationCallback{};
}

#include "moc_notification-callback-repository.cpp"
