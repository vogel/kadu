/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "configuration/configuration-file.h"
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

	config_file.writeEntry(section, QString("Event_") + EventName + name, value.value<QString>());
}

QVariant NotifierConfigurationDataManager::readEntry(const QString &section, const QString &name)
{
	if (section.isEmpty() || name.isEmpty())
		return QVariant(QString::null);

	return config_file.readEntry(section, QString("Event_") + EventName + name);
}

NotifierConfigurationDataManager * NotifierConfigurationDataManager::dataManagerForEvent(const QString &eventName)
{
	if (DataManagers[eventName])
		return DataManagers[eventName];
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
