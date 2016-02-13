/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2010, 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtWidgets/QApplication>

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/injected-factory.h"
#include "gui/windows/message-dialog.h"
#include "notification/notification-service.h"
#include "notification/notification.h"
#include "notification/notifier-repository.h"
#include "notification/notifier.h"
#include "protocols/protocol.h"
#include "debug.h"

#include "notification/notification-manager.h"

NotificationManager::NotificationManager(QObject *parent) :
		QObject{parent}
{
}

NotificationManager::~NotificationManager()
{
}

void NotificationManager::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

QString NotificationManager::notifyConfigurationKey(const QString &eventType)
{
	QString event = eventType;

	while (true)
	{
		int slashPosition = event.lastIndexOf('/');
		if (-1 == slashPosition)
			return event;

		if (m_configuration->deprecatedApi()->readBoolEntry("Notify", event + "_UseCustomSettings", false))
			return event;

		event = event.left(slashPosition);
	}

	Q_ASSERT(false);
}

#include "moc_notification-manager.cpp"
