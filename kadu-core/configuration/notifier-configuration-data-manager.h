/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef NOTIFIER_CONFIGURATION_DATA_MANAGER_H
#define NOTIFIER_CONFIGURATION_DATA_MANAGER_H

#include "configuration/configuration-window-data-manager.h"

#include "exports.h"

class ConfigurationWindow;

class KADUAPI NotifierConfigurationDataManager : public ConfigurationWindowDataManager
{
	Q_OBJECT

	QString EventName;
	int UsageCount;

	static QMap<QString, NotifierConfigurationDataManager*> DataManagers;
	static void dataManagerDestroyed(const QString &eventName);

	explicit NotifierConfigurationDataManager(const QString &eventName, QObject *parent = 0);

private slots:
	void configurationWindowDestroyed();

public:

	virtual void writeEntry(const QString &section, const QString &name, const QVariant &value);
	virtual QVariant readEntry(const QString &section, const QString &name);

	static NotifierConfigurationDataManager * dataManagerForEvent(const QString &eventName);

	void configurationWindowCreated(ConfigurationWindow *window);
};

#endif //NOTIFIER_CONFIGURATION_DATA_MANAGER_H
