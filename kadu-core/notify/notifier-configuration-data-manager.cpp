/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/application.h"
#include "gui/windows/configuration-window.h"

#include "notifier-configuration-data-manager.h"

QMap<QString, NotifierConfigurationDataManager *> NotifierConfigurationDataManager::DataManagers;

NotifierConfigurationDataManager::NotifierConfigurationDataManager(const QString &eventName, QObject *parent)
	: ConfigurationWindowDataManager(parent), EventName(eventName), UsageCount(0)
{
}

void NotifierConfigurationDataManager::writeEntry(const QString &section, const QString &name, const QVariant &value)
{
	if (section.isEmpty() || name.isEmpty())
		return;

	Application::instance()->configuration()->deprecatedApi()->writeEntry(section, QString("Event_") + EventName + name, value.toString());
}

QVariant NotifierConfigurationDataManager::readEntry(const QString &section, const QString &name)
{
	if (section.isEmpty() || name.isEmpty())
		return QVariant(QString());

	return Application::instance()->configuration()->deprecatedApi()->readEntry(section, QString("Event_") + EventName + name);
}

NotifierConfigurationDataManager * NotifierConfigurationDataManager::dataManagerForEvent(const QString &eventName)
{
	if (DataManagers.contains(eventName))
		return DataManagers.value(eventName);
	else
		return DataManagers[eventName] = new NotifierConfigurationDataManager(eventName);
}

void NotifierConfigurationDataManager::dataManagerDestroyed(const QString &eventName)
{
	DataManagers.remove(eventName);
}

void NotifierConfigurationDataManager::configurationWindowCreated(ConfigurationWindow *window)
{
	connect(window, SIGNAL(destroyed()), this, SLOT(configurationWindowDestroyed()));
	++UsageCount;
}

void NotifierConfigurationDataManager::configurationWindowDestroyed()
{
	if (!--UsageCount)
	{
		NotifierConfigurationDataManager::dataManagerDestroyed(EventName);
		deleteLater();
	}
}

#include "moc_notifier-configuration-data-manager.cpp"
