/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QApplication>
#include <QtGui/QCheckBox>
#include <QtGui/QGridLayout>
#include <QtGui/QPushButton>
#include <QtGui/QHBoxLayout>
#include <QtGui/QListWidget>
#include <QtGui/QLabel>

#include "configuration/configuration-file.h"
#include "buddies/buddy-list.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy-shared.h"
#include "contacts/contact.h"
#include "gui/widgets/chat-widget.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/widgets/configuration/config-combo-box.h"
#include "gui/widgets/configuration/config-group-box.h"
#include "gui/widgets/configuration/notify-group-box.h"
#include "gui/widgets/configuration/notifier-configuration-widget.h"
#include "gui/widgets/configuration/notify-tree-widget.h"
#include "gui/windows/configuration-window.h"

#include "buddy-notify-data.h"
#include "notifier.h"
#include "notify-event.h"

#include "notify-configuration-ui-handler.h"

#include "debug.h"

NotifyConfigurationUiHandler::NotifyConfigurationUiHandler(QObject *parent) :
		QObject(parent), notificationsGroupBox(0)
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
			qApp->translate("@default", notifier->description().toAscii().data()), notificationsGroupBox->widget());
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
	connect(mainConfigurationWindow, SIGNAL(destroyed()), this, SLOT(mainConfigurationWindowDestroyed()));

	foreach (Notifier *notifier, NotificationManager::instance()->notifiers())
	{
		foreach (NotifyEvent *notifyEvent, NotificationManager::instance()->notifyEvents())
		{
			if (!NotifierGui[notifier].Events.contains(notifyEvent->name()))
				NotifierGui[notifier].Events[notifyEvent->name()] = config_file.readBoolEntry("Notify", notifyEvent->name() + '_' + notifier->name());
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
		item.useCustomSettings = config_file.readBoolEntry("Notify", eventName + "_UseCustomSettings", false);

		NotifyEvents[eventName] = item;
	}

	ConfigGroupBox *statusGroupBox = mainConfigurationWindow->widget()->configGroupBox("Notifications", "Options", "Status change");

	QWidget *notifyUsers = new QWidget(statusGroupBox->widget());
	QGridLayout *notifyUsersLayout = new QGridLayout(notifyUsers);
	notifyUsersLayout->setSpacing(5);
	notifyUsersLayout->setMargin(5);

	allUsers = new QListWidget(notifyUsers);
	QPushButton *moveToNotifyList = new QPushButton(tr("Move to 'Notify List'"), notifyUsers);

	notifyUsersLayout->addWidget(new QLabel(tr("Buddy List"), notifyUsers), 0, 0);
	notifyUsersLayout->addWidget(allUsers, 1, 0);
	notifyUsersLayout->addWidget(moveToNotifyList, 2, 0);

	notifiedUsers = new QListWidget(notifyUsers);
	QPushButton *moveToAllList = new QPushButton(tr("Move to 'Buddy List'"), notifyUsers);

	notifyUsersLayout->addWidget(new QLabel(tr("Notify List"), notifyUsers), 0, 1);
	notifyUsersLayout->addWidget(notifiedUsers, 1, 1);
	notifyUsersLayout->addWidget(moveToAllList, 2, 1);

	connect(moveToNotifyList, SIGNAL(clicked()), this, SLOT(moveToNotifyList()));
	connect(moveToAllList, SIGNAL(clicked()), this, SLOT(moveToAllList()));

	statusGroupBox->addWidgets(0, notifyUsers);

	foreach (const Buddy &buddy, BuddyManager::instance()->items())
		if (!buddy.isAnonymous())
		{
			BuddyNotifyData *bnd = 0;
			if (buddy.data())
				bnd = buddy.data()->moduleStorableData<BuddyNotifyData>("notify", NotificationManager::instance(), false);

			if (!bnd || !bnd->notify())
				allUsers->addItem(buddy.display());
			else
				notifiedUsers->addItem(buddy.display());
		}

	allUsers->sortItems();
	notifiedUsers->sortItems();
	allUsers->setSelectionMode(QAbstractItemView::ExtendedSelection);
	notifiedUsers->setSelectionMode(QAbstractItemView::ExtendedSelection);

	connect(notifiedUsers, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(moveToAllList()));
	connect(allUsers, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(moveToNotifyList()));

	connect(mainConfigurationWindow->widget()->widgetById("notify/notifyAll"), SIGNAL(toggled(bool)), notifyUsers, SLOT(setDisabled(bool)));
	connect(mainConfigurationWindow, SIGNAL(configurationWindowApplied()), this, SLOT(configurationWindowApplied()));

	notificationsGroupBox = mainConfigurationWindow->widget()->configGroupBox("Notifications", "General", "Notifications");

	notifyTreeWidget = new NotifyTreeWidget(this, notificationsGroupBox->widget());
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
			item.useCustomSettings = config_file.readBoolEntry("Notify", eventName + "_UseCustomSettings", false);
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
	int count = notifiedUsers->count();
	for (int i = 0; i < count; i++)
	{
		Buddy buddy = BuddyManager::instance()->byDisplay(notifiedUsers->item(i)->text(), ActionReturnNull);
		if (buddy.isNull() || buddy.isAnonymous())
			continue;

		BuddyNotifyData *bnd = 0;
		if (buddy.data())
			bnd = buddy.data()->moduleStorableData<BuddyNotifyData>("notify", NotificationManager::instance(), true);
		if (!bnd)
			continue;

		bnd->setNotify(true);
		bnd->store();
	}

	count = allUsers->count();
	for (int i = 0; i < count; i++)
	{
		Buddy buddy = BuddyManager::instance()->byDisplay(allUsers->item(i)->text(), ActionReturnNull);
		if (buddy.isNull() || buddy.isAnonymous())
			continue;

		BuddyNotifyData *bnd = 0;
		if (buddy.data())
			bnd = buddy.data()->moduleStorableData<BuddyNotifyData>("notify", NotificationManager::instance(), true);
		if (!bnd)
			continue;

		bnd->setNotify(false);
		bnd->store();
	}

	foreach (NotifyEvent *notifyEvent, NotificationManager::instance()->notifyEvents())
	{
		if (notifyEvent->category().isEmpty() || !NotifyEvents.contains(notifyEvent->name()))
			continue;

		config_file.writeEntry("Notify", notifyEvent->name() + "_UseCustomSettings", NotifyEvents[notifyEvent->name()].useCustomSettings);
	}

	foreach (Notifier *notifier, NotificationManager::instance()->notifiers())
	{
		if (!NotifierGui.contains(notifier))
			continue;

		NotifierConfigurationGuiItem &gui = NotifierGui[notifier];
		if (gui.ConfigurationWidget)
			gui.ConfigurationWidget->saveNotifyConfigurations();

		foreach (const QString &eventKey, gui.Events.keys())
			config_file.writeEntry("Notify", eventKey + '_' + notifier->name(), gui.Events[eventKey]);
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
			gui.Events[CurrentEvent] = config_file.readBoolEntry("Notify", CurrentEvent + '_' + notifier->name());

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
