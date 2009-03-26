/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef NOTIFY_CONFIGURATION_UI_HANDLER
#define NOTIFY_CONFIGURATION_UI_HANDLER

#include "main_configuration_window.h"

#include "notification-manager.h"

class QListWidget;

class NotifyConfigurationUiHandler : public ConfigurationUiHandler
{
	Q_OBJECT

	QListWidget *allUsers;
	QListWidget *notifiedUsers;
	ConfigComboBox *notifications;
	ConfigGroupBox *notificationsGroupBox;

	QString CurrentEvent;

	void registerNotifier(const QString &name);
	void unregisterNotifier(const QString &name);

	void addConfigurationWidget(NotificationManager::NotifierData &notifier, const QString &name);
	void removeConfigurationWidget(NotificationManager::NotifierData &notifier);

	friend class NotificationManager;

private slots:
	void configurationWindowApplied();
	void mainConfigurationWindowDestroyed();

	void moveToNotifyList();
	void moveToAllList();

	void eventSwitched(int index);
	void notifierToggled(const QString &notifier, bool toggled);

protected:
	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

public:
	explicit NotifyConfigurationUiHandler(QObject *parent = 0);
	virtual ~NotifyConfigurationUiHandler();

};

#endif // NOTIFY_CONFIGURATION_UI_HANDLER
