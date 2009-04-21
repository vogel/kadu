/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QApplication>
#include <QtGui/QGridLayout>
#include <QtGui/QPushButton>
#include <QtGui/QHBoxLayout>
#include <QtGui/QListWidget>
#include <QtGui/QLabel>

#include "contacts/contact-account-data.h"
#include "contacts/contact-list.h"
#include "contacts/contact-manager.h"
#include "contact-notify-data.h"
#include "gui/widgets/chat-widget.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/widgets/configuration/config-combo-box.h"
#include "gui/widgets/configuration/config-group-box.h"
#include "gui/widgets/configuration/notify-group-box.h"
#include "gui/widgets/configuration/notifier-configuration-widget.h"
#include "gui/widgets/configuration/notify-tree-widget.h"
#include "gui/windows/configuration-window.h"
#include "notify/notifier.h"
#include "notify/notify-event.h"

#include "notify-configuration-ui-handler.h"

#include "config_file.h"
#include "debug.h"

NotifyConfigurationUiHandler::NotifyConfigurationUiHandler(QObject *parent) :
		QObject(parent), notificationsGroupBox(0)
{
	connect(NotificationManager::instance(), SIGNAL(notiferRegistered(Notifier *)),
			this, SLOT(notifierRegistered(Notifier *)));
	connect(NotificationManager::instance(), SIGNAL(notiferUnregistered(Notifier *)),
			this, SLOT(notifierUnregistered(Notifier *)));
}

NotifyConfigurationUiHandler::~NotifyConfigurationUiHandler()
{

}

void NotifyConfigurationUiHandler::addConfigurationWidget(Notifier *notifier)
{
	NotifyGroupBox *configurationGroupBox = new NotifyGroupBox(notifier,
			qApp->translate("@default", notifier->name().toAscii().data()), notificationsGroupBox->widget());
	connect(configurationGroupBox, SIGNAL(toggled(Notifier *, bool)), this, SLOT(notifierToggled(Notifier *, bool)));

	if (!NotifierGui.contains(notifier))
		NotifierGui.insert(notifier, NotifierGuiItem());

	NotifierGui[notifier].ConfigurationGroupBox = configurationGroupBox;

	NotifierConfigurationWidget *notifyConfigurationWidget = notifier->createConfigurationWidget(configurationGroupBox);
	if (notifyConfigurationWidget)
	{
		NotifierGui[notifier].ConfigurationWidget = notifyConfigurationWidget;
		notifyConfigurationWidget->loadNotifyConfigurations();
	}
//	else
//	{
//		configurationGroupBox->setFlat(true);
//		configurationGroupBox->setLineWidth(0);
//	}

	notificationsGroupBox->addWidget(configurationGroupBox, true);
	configurationGroupBox->show();
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
	connect(mainConfigurationWindow, SIGNAL(destroyed(QObject *)), this, SLOT(mainConfigurationWindowDestroyed()));

	foreach (Notifier *notifier, NotificationManager::instance()->notifiers())
	{
		foreach (NotifyEvent *notifyEvent, NotificationManager::instance()->notifyEvents())
		{
			if (!NotifierGui[notifier].Events.contains(notifyEvent->name()))
				NotifierGui[notifier].Events[notifyEvent->name()] = config_file.readBoolEntry("Notify", notifyEvent->name() + '_' + notifier->name());
		}
	}

	ConfigGroupBox *statusGroupBox = mainConfigurationWindow->widget()->configGroupBox("Notifications", "Options", "Status change");

	QWidget *notifyUsers = new QWidget(statusGroupBox->widget());
	QGridLayout *notifyUsersLayout = new QGridLayout(notifyUsers);
	notifyUsersLayout->setSpacing(5);
	notifyUsersLayout->setMargin(5);

	allUsers = new QListWidget(notifyUsers);
	QPushButton *moveToNotifyList = new QPushButton(tr("Move to 'Notify list'"), notifyUsers);

	notifyUsersLayout->addWidget(new QLabel(tr("User list"), notifyUsers), 0, 0);
	notifyUsersLayout->addWidget(allUsers, 1, 0);
	notifyUsersLayout->addWidget(moveToNotifyList, 2, 0);

	notifiedUsers = new QListWidget(notifyUsers);
	QPushButton *moveToAllList = new QPushButton(tr("Move to 'User list'"), notifyUsers);

	notifyUsersLayout->addWidget(new QLabel(tr("Notify list"), notifyUsers), 0, 1);
	notifyUsersLayout->addWidget(notifiedUsers, 1, 1);
	notifyUsersLayout->addWidget(moveToAllList, 2, 1);

	connect(moveToNotifyList, SIGNAL(clicked()), this, SLOT(moveToNotifyList()));
	connect(moveToAllList, SIGNAL(clicked()), this, SLOT(moveToAllList()));

	statusGroupBox->addWidgets(0, notifyUsers);

	foreach(Contact contact, ContactManager::instance()->contacts())
		if (!contact.isAnonymous())
		{
			ContactNotifyData *cnd = contact.moduleData<ContactNotifyData>();

			if (!cnd || !cnd->notify())
				allUsers->addItem(contact.display());
			else
				notifiedUsers->addItem(contact.display());

			delete cnd;
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

	notifyTreeWidget = new NotifyTreeWidget(NotifierGui, notificationsGroupBox->widget());
	notificationsGroupBox->addWidget(notifyTreeWidget, true);
	notifyTreeWidget->setCurrentItem(notifyTreeWidget->topLevelItem(0));
	connect(notifyTreeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(eventSwitched()));

	foreach (Notifier *notifier, NotificationManager::instance()->notifiers())
		addConfigurationWidget(notifier);

	eventSwitched();
}

void NotifyConfigurationUiHandler::notifierRegistered(Notifier *notifier)
{
	if (notificationsGroupBox)
	{
		addConfigurationWidget(notifier);
		notifyTreeWidget->refresh(NotifierGui);
	}
}

void NotifyConfigurationUiHandler::notifierUnregistered(Notifier *notifier)
{
	if (notificationsGroupBox)
		removeConfigurationWidget(notifier);

	if (NotifierGui.contains(notifier))
		NotifierGui.remove(notifier);

	if (notificationsGroupBox)
		notifyTreeWidget->refresh(NotifierGui);
}

void NotifyConfigurationUiHandler::configurationWindowApplied()
{
	int count = notifiedUsers->count();
	for (int i = 0; i < count; i++)
	{
		Contact contact = ContactManager::instance()->byDisplay(notifiedUsers->item(i)->text());
		if (contact.isNull() || contact.isAnonymous())
			continue;

		ContactNotifyData *cnd = contact.moduleData<ContactNotifyData>();
		if (!cnd)
			continue;

		cnd->setNotify(true);
		cnd->storeConfiguration();
		delete cnd;
	}

	count = allUsers->count();
	for (int i = 0; i < count; i++)
	{
		Contact contact = ContactManager::instance()->byDisplay(allUsers->item(i)->text());
		if (contact.isNull() || contact.isAnonymous())
			continue;

		ContactNotifyData *cnd = contact.moduleData<ContactNotifyData>();
		if (!cnd)
			continue;

		cnd->setNotify(false);
		cnd->storeConfiguration();
		delete cnd;
	}

	foreach (Notifier *notifier, NotificationManager::instance()->notifiers())
	{
		if (!NotifierGui.contains(notifier))
			continue;

		NotifierGuiItem &gui = NotifierGui[notifier];
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
			QListWidgetItem *it = allUsers->takeItem(i);
			delete it;
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
			QListWidgetItem *it = notifiedUsers->takeItem(i);
			delete it;
		}

	allUsers->sortItems();
}

void NotifyConfigurationUiHandler::eventSwitched()
{
	kdebugf();

	CurrentEvent = notifyTreeWidget->currentEvent();

	foreach (Notifier *notifier, NotificationManager::instance()->notifiers())
	{
		if (!NotifierGui.contains(notifier))
			NotifierGui.insert(notifier, NotifierGuiItem());

		NotifierGuiItem &gui = NotifierGui[notifier];

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
		NotifierGui.insert(notifier, NotifierGuiItem());
	NotifierGui[notifier].Events[CurrentEvent] = toggled;
	notifyTreeWidget->updateCurrentItem(notifier, toggled);
}
