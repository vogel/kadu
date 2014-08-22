/*
 * %kadu copyright begin%
 * Copyright 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef NOTIFY_CONFIGURATION_UI_HANDLER
#define NOTIFY_CONFIGURATION_UI_HANDLER

#include "gui/windows/main-configuration-window.h"

#include "notification-manager.h"

class QCheckBox;
class QListWidget;

class NotifierConfigurationWidget;
class NotifyGroupBox;
class NotifyTreeWidget;

struct NotifierConfigurationGuiItem
{
	NotifierConfigurationWidget *ConfigurationWidget;
	NotifyGroupBox *ConfigurationGroupBox;
	QMap<QString, bool> Events;

	NotifierConfigurationGuiItem() : ConfigurationWidget(0), ConfigurationGroupBox(0) {}
};

struct NotifyEventConfigurationItem
{
	NotifyEvent *event;
	bool useCustomSettings;
};

class NotifyConfigurationUiHandler : public ConfigurationUiHandler
{
	Q_OBJECT

	QMap<Notifier *, NotifierConfigurationGuiItem> NotifierGui;
	QMap<QString, NotifyEventConfigurationItem> NotifyEvents;

	QListWidget *allUsers;
	QListWidget *notifiedUsers;
	ConfigGroupBox *notificationsGroupBox;

	QCheckBox *useCustomSettingsCheckBox;
	NotifyTreeWidget *notifyTreeWidget;

	QString CurrentEvent;

	void addConfigurationWidget(Notifier *notifier);
	void removeConfigurationWidget(Notifier *notifier);

private slots:
	void notifierRegistered(Notifier *notifier);
	void notifierUnregistered(Notifier *notifier);

	void notifyEventRegistered(NotifyEvent *notifyEvent);
	void notifyEventUnregistered(NotifyEvent *notifyEvent);

	void configurationWindowApplied();
	void mainConfigurationWindowDestroyed();

	void moveToNotifyList();
	void moveToAllList();

	void eventSwitched();
	void notifierToggled(Notifier *notifier, bool toggled);

	void customSettingsCheckBoxToggled(bool toggled);

protected:
	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

public:
	explicit NotifyConfigurationUiHandler(QObject *parent = 0);
	virtual ~NotifyConfigurationUiHandler();

	const QMap<Notifier *, NotifierConfigurationGuiItem> & notifierGui() { return NotifierGui; }
	const QMap<QString, NotifyEventConfigurationItem> & notifyEvents() { return NotifyEvents; }
};

#endif // NOTIFY_CONFIGURATION_UI_HANDLER
