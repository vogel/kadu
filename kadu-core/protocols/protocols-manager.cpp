/*
 * %kadu copyright begin%
 * Copyright 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "accounts/account-manager.h"
#include "configuration/configuration-api.h"
#include "configuration/configuration.h"
#include "misc/misc.h"
#include "notification/notification-callback-repository.h"
#include "notification/notification-event-repository.h"
#include "protocols/connection-error-notification.h"
#include "protocols/protocol-factory.h"

#include "debug.h"

#include "protocols-manager.h"

ProtocolsManager::ProtocolsManager(QObject *parent) :
		QObject{parent}
{
}

ProtocolsManager::~ProtocolsManager()
{
}

void ProtocolsManager::setNotificationCallbackRepository(NotificationCallbackRepository *notificationCallbackRepository)
{
	m_notificationCallbackRepository = notificationCallbackRepository;
}

void ProtocolsManager::setNotificationEventRepository(NotificationEventRepository *notificationEventRepository)
{
	m_notificationEventRepository = notificationEventRepository;
}

void ProtocolsManager::init()
{
	ConnectionErrorNotification::registerEvent(m_notificationEventRepository, m_notificationCallbackRepository);
}

void ProtocolsManager::done()
{
	ConnectionErrorNotification::unregisterEvent(m_notificationEventRepository);
}

void ProtocolsManager::registerProtocolFactory(ProtocolFactory *factory)
{
	if (!factory || Factories.contains(factory))
		return;

	emit protocolFactoryAboutToBeRegistered(factory);
	Factories.append(factory);
	emit protocolFactoryRegistered(factory);
}

void ProtocolsManager::unregisterProtocolFactory(ProtocolFactory *factory)
{
	if (!factory || !Factories.contains(factory))
		return;

	emit protocolFactoryAboutToBeUnregistered(factory);
	Factories.removeAll(factory);
	emit protocolFactoryUnregistered(factory);
}

bool ProtocolsManager::hasProtocolFactory(const QString &name)
{
	foreach (ProtocolFactory *factory, Factories)
		if (factory->name() == name)
			return true;

	return false;
}

ProtocolFactory * ProtocolsManager::byName(const QString &name)
{
	foreach (ProtocolFactory *factory, Factories)
		if (factory->name() == name)
			return factory;

	return 0;
}

ProtocolFactory * ProtocolsManager::byIndex(int index)
{
	if (index < 0 || index >= Factories.count())
		return 0;

	return Factories.at(index);
}

#include "moc_protocols-manager.cpp"
