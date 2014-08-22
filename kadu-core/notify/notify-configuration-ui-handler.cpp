/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2009, 2010, 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>

#include "buddies/buddy-list.h"
#include "buddies/buddy-manager.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "contacts/contact.h"
#include "core/application.h"
#include "gui/widgets/chat-widget/chat-widget.h"
#include "gui/widgets/configuration/config-combo-box.h"
#include "gui/widgets/configuration/config-group-box.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/widgets/configuration/notifier-configuration-widget.h"
#include "gui/widgets/configuration/notify-group-box.h"
#include "gui/widgets/configuration/notify-tree-widget.h"
#include "gui/windows/configuration-window.h"

#include "notifier.h"
#include "notify-event.h"

#include "notify-configuration-ui-handler.h"

#include "debug.h"

NotifyConfigurationUiHandler::NotifyConfigurationUiHandler(QObject *parent) :
		ConfigurationUiHandler{parent}, allUsers{}, notifiedUsers{}, notificationsGroupBox{},
		useCustomSettingsCheckBox{}, notifyTreeWidget{}
{
	connect(NotificationManager::instance(), SIGNAL(notiferRegistered(Notifier *)),
			this, SLOT(notifierRegistered(Notifier *)));
	connect(NotificationManager::instance(), SIGNAL(notiferUnregistered(Notifier *)),
			this, SLOT(notifierUnregistered(Notifier *)));

	connect(NotificationManager::instance(), SIGNAL(notifyEventRegistered(NotifyEvent *)),
			this, SLOT(notifyEventRegistered(NotifyEvent *)));
	connect(NotificationManager::instance(), SIGNAL(notifyEventUnregistered(NotifyEvent *)),
			this, SLOT(notifyEventUnregistered(NotifyEvent *)));
}

NotifyConfigurationUiHandler::~NotifyConfigurationUiHandler()
{

}

void NotifyConfigurationUiHandler::addConfigurationWidget(Notifier *notifier)
{
	NotifyGroupBox *configurationGroupBox = new NotifyGroupBox(notifier,
			QCoreApplication::translate("@default", notifier->description().toUtf8().constData()), notificationsGroupBox->widget());
	connect(configurationGroupBox, SIGNAL(toggled(Notifier *, bool)), this, SLOT(notifierToggled(Notifier *, bool)));
	if (!NotifierGui.contains(notifier))
		NotifierGui.insert(notifier, NotifierConfigurationGuiItem());

	NotifierGui[notifier].ConfigurationGroupBox = configurationGroupBox;

	NotifierConfigurationWidget *notifyConfigurationWidget = notifier->createConfigurationWidget(configurationGroupBox);
	if (notifyConfigurationWidget)
	{
		NotifierGui[notifier].ConfigurationWidget = notifyConfigurationWidget;
		notifyConfigurationWidget->loadNotifyConfigurations();
	}

	notificationsGroupBox->addWidget(configurationGroupBox, true);
}

void NotifyConfigurationUiHandler::removeConfigurationWidget(Notifier *notifier)
{
	if (!NotifierGui.contains(notifier))
		return;

	if (NotifierGui[notifier].ConfigurationWidget)
	{
		delete NotifierGui[notifier].ConfigurationWidget;
		NotifierGui[notifier].ConfigurationWidget = 0;
	}

	delete NotifierGui[notifier].ConfigurationGroupBox;
	NotifierGui[notifier].ConfigurationGroupBox = 0;

	NotifierGui.remove(notifier);
}

void NotifyConfigurationUiHandler::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	connect(mainConfigurationWindow, SIGNAL(configurationWindowApplied()),
			this, SLOT(configurationWindowApplied()));
	connect(mainConfigurationWindow, SIGNAL(destroyed()), this, SLOT(mainConfigurationWindowDestroyed()));

	foreach (Notifier *notifier, NotificationManager::instance()->notifiers())
	{
		foreach (NotifyEvent *notifyEvent, NotificationManager::instance()->notifyEvents())
		{
			if (!NotifierGui[notifier].Events.contains(notifyEvent->name()))
				NotifierGui[notifier].Events[notifyEvent->name()] = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Notify", notifyEvent->name() + '_' + notifier->name());
		}
	}

	QString eventName;
	foreach (NotifyEvent *notifyEvent, NotificationManager::instance()->notifyEvents())
	{
		eventName = notifyEvent->name();
		if (NotifyEvents.contains(eventName))
			continue;

		NotifyEventConfigurationItem item;
		item.event = notifyEvent;
		item.useCustomSettings = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Notify", eventName + "_UseCustomSettings", false);

		NotifyEvents[eventName] = item;
	}

	notificationsGroupBox = mainConfigurationWindow->widget()->configGroupBox("Notifications", "Events", "Notifications");

	notifyTreeWidget = new NotifyTreeWidget(this, notificationsGroupBox->widget());
	notifyTreeWidget->setMinimumHeight(250);
	notificationsGroupBox->addWidget(notifyTreeWidget, true);
	notifyTreeWidget->setCurrentItem(notifyTreeWidget->topLevelItem(0));
	connect(notifyTreeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(eventSwitched()));

	useCustomSettingsCheckBox = new QCheckBox(tr("Use custom settings"));
	connect(useCustomSettingsCheckBox, SIGNAL(toggled(bool)), this, SLOT(customSettingsCheckBoxToggled(bool)));
	notificationsGroupBox->addWidget(useCustomSettingsCheckBox, true);

	foreach (Notifier *notifier, NotificationManager::instance()->notifiers())
		addConfigurationWidget(notifier);

	eventSwitched();
}

void NotifyConfigurationUiHandler::notifierRegistered(Notifier *notifier)
{
	if (notificationsGroupBox)
	{
		addConfigurationWidget(notifier);
		notifyTreeWidget->refresh();
	}
}

void NotifyConfigurationUiHandler::notifierUnregistered(Notifier *notifier)
{
	if (notificationsGroupBox)
		removeConfigurationWidget(notifier);

	if (NotifierGui.contains(notifier))
		NotifierGui.remove(notifier);

	if (notificationsGroupBox)
		notifyTreeWidget->refresh();
}

void NotifyConfigurationUiHandler::notifyEventRegistered(NotifyEvent *notifyEvent)
{
	if (notificationsGroupBox)
	{
		QString eventName = notifyEvent->name();
		NotifyEventConfigurationItem item;
		item.event = notifyEvent;
		if (!notifyEvent->category().isEmpty())
			item.useCustomSettings = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Notify", eventName + "_UseCustomSettings", false);
		else
			item.useCustomSettings = true;

		NotifyEvents[eventName] = item;

		notifyTreeWidget->refresh();
	}
}

void NotifyConfigurationUiHandler::notifyEventUnregistered(NotifyEvent *notifyEvent)
{
	if (NotifyEvents.contains(notifyEvent->name()))
		NotifyEvents.remove(notifyEvent->name());

	if (notificationsGroupBox)
		notifyTreeWidget->refresh();
}

void NotifyConfigurationUiHandler::configurationWindowApplied()
{
	foreach (NotifyEvent *notifyEvent, NotificationManager::instance()->notifyEvents())
	{
		if (notifyEvent->category().isEmpty() || !NotifyEvents.contains(notifyEvent->name()))
			continue;

		Application::instance()->configuration()->deprecatedApi()->writeEntry("Notify", notifyEvent->name() + "_UseCustomSettings", NotifyEvents[notifyEvent->name()].useCustomSettings);
	}

	foreach (Notifier *notifier, NotificationManager::instance()->notifiers())
	{
		if (!NotifierGui.contains(notifier))
			continue;

		NotifierConfigurationGuiItem &gui = NotifierGui[notifier];
		if (gui.ConfigurationWidget)
			gui.ConfigurationWidget->saveNotifyConfigurations();

		for (QMap<QString, bool>::const_iterator it = gui.Events.constBegin(), end = gui.Events.constEnd(); it != end; ++it)
			Application::instance()->configuration()->deprecatedApi()->writeEntry("Notify", it.key() + '_' + notifier->name(), it.value());
	}
}

void NotifyConfigurationUiHandler::mainConfigurationWindowDestroyed()
{
	notificationsGroupBox = 0;
	NotifierGui.clear();
}

void NotifyConfigurationUiHandler::moveToNotifyList()
{
	int count = allUsers->count();

	for (int i = count - 1; i >= 0; i--)
		if (allUsers->item(i)->isSelected())
		{
			notifiedUsers->addItem(allUsers->item(i)->text());
			delete allUsers->takeItem(i);
		}

	notifiedUsers->sortItems();
}

void NotifyConfigurationUiHandler::moveToAllList()
{
	int count = notifiedUsers->count();

	for (int i = count - 1; i >= 0; i--)
		if (notifiedUsers->item(i)->isSelected())
		{
			allUsers->addItem(notifiedUsers->item(i)->text());
			delete notifiedUsers->takeItem(i);
		}

	allUsers->sortItems();
}

void NotifyConfigurationUiHandler::eventSwitched()
{
	kdebugf();

	CurrentEvent = notifyTreeWidget->currentEvent();

	useCustomSettingsCheckBox->setVisible(!NotifyEvents[CurrentEvent].event->category().isEmpty());
	useCustomSettingsCheckBox->setChecked(NotifyEvents[CurrentEvent].useCustomSettings);
	customSettingsCheckBoxToggled(useCustomSettingsCheckBox->isHidden() || NotifyEvents[CurrentEvent].useCustomSettings);

	foreach (Notifier *notifier, NotificationManager::instance()->notifiers())
	{
		if (!NotifierGui.contains(notifier))
			NotifierGui.insert(notifier, NotifierConfigurationGuiItem());

		NotifierConfigurationGuiItem &gui = NotifierGui[notifier];

		if (!gui.Events.contains(CurrentEvent))
			gui.Events[CurrentEvent] = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Notify", CurrentEvent + '_' + notifier->name());

		if (gui.ConfigurationWidget)
			gui.ConfigurationWidget->switchToEvent(CurrentEvent);

		if (gui.ConfigurationGroupBox)
			gui.ConfigurationGroupBox->setChecked(gui.Events[CurrentEvent]);
	}
}

void NotifyConfigurationUiHandler::notifierToggled(Notifier *notifier, bool toggled)
{
	kdebugf();

	if (!NotifierGui.contains(notifier))
		NotifierGui.insert(notifier, NotifierConfigurationGuiItem());
	NotifierGui[notifier].Events[CurrentEvent] = toggled;

	notifyTreeWidget->notifierChecked(notifier, toggled);
}

void NotifyConfigurationUiHandler::customSettingsCheckBoxToggled(bool toggled)
{
	NotifyEvents[CurrentEvent].useCustomSettings = toggled;

	notifyTreeWidget->useCustomSettingsChecked(toggled);

	foreach (const NotifierConfigurationGuiItem &guiItem, NotifierGui)
		if (guiItem.ConfigurationGroupBox)
			guiItem.ConfigurationGroupBox->setVisible(toggled);
}

#include "moc_notify-configuration-ui-handler.cpp"
