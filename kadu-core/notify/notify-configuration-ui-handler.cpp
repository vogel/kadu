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
#include "gui/widgets/chat_widget.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/widgets/configuration/config-combo-box.h"
#include "gui/widgets/configuration/config-group-box.h"
#include "gui/widgets/configuration/notify-group-box.h"
#include "gui/widgets/configuration/notifier-configuration-widget.h"
#include "gui/windows/configuration-window.h"
#include "notify/notify-event.h"

#include "notify-configuration-ui-handler.h"

#include "config_file.h"
#include "debug.h"

NotifyConfigurationUiHandler::NotifyConfigurationUiHandler(QObject *parent) :
		QObject(parent), notificationsGroupBox(0)
{
}

NotifyConfigurationUiHandler::~NotifyConfigurationUiHandler()
{

}

void NotifyConfigurationUiHandler::addConfigurationWidget(NotificationManager::NotifierData &notifier, const QString &name)
{
	NotifyGroupBox *configurationGroupBox = new NotifyGroupBox(name, qApp->translate("@default", name.toAscii().data()), notificationsGroupBox->widget());
	connect(configurationGroupBox, SIGNAL(toggled(const QString &, bool)), this, SLOT(notifierToggled(const QString &, bool)));

	notifier.configurationGroupBox = configurationGroupBox;

	NotifierConfigurationWidget *notifyConfigurationWidget = notifier.notifier->createConfigurationWidget(configurationGroupBox);
	if (notifyConfigurationWidget)
	{
		notifier.configurationWidget = notifyConfigurationWidget;
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

void NotifyConfigurationUiHandler::removeConfigurationWidget(NotificationManager::NotifierData &notifier)
{
	if (notifier.configurationWidget)
	{
		delete notifier.configurationWidget;
		notifier.configurationWidget = 0;
	}

	delete notifier.configurationGroupBox;
	notifier.configurationGroupBox = 0;
}

void NotifyConfigurationUiHandler::registerNotifier(const QString &name)
{
	if (notificationsGroupBox)
		addConfigurationWidget(NotificationManager::instance()->Notifiers[name], name);
}

void NotifyConfigurationUiHandler::unregisterNotifier(const QString &name)
{
	if (notificationsGroupBox)
		removeConfigurationWidget(NotificationManager::instance()->Notifiers[name]);
}

void NotifyConfigurationUiHandler::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	connect(mainConfigurationWindow, SIGNAL(destroyed()), this, SLOT(mainConfigurationWindowDestroyed()));

	notifications = dynamic_cast<ConfigComboBox *>(mainConfigurationWindow->widget()->widgetById("notify/notifications"));
	connect(notifications, SIGNAL(activated(int)), this, SLOT(eventSwitched(int)));

	QStringList captions;
	QStringList values;

	foreach (NotifyEvent *notifyEvent, NotificationManager::instance()->notifyEvents())
	{
		captions.append(qApp->translate("@default", notifyEvent->description()));
		values.append(notifyEvent->name());
	}

	notifications->setItems(values, captions);

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

	// TODO 0.6.6 display -> uuid?
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

	foreach(const QString &key, NotificationManager::instance()->Notifiers.keys())
		addConfigurationWidget(NotificationManager::instance()->Notifiers[key], key);

	eventSwitched(0);
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

	foreach (const QString &key, NotificationManager::instance()->Notifiers.keys())
	{
		NotificationManager::NotifierData notifierData = NotificationManager::instance()->Notifiers[key];

		if (notifierData.configurationWidget)
			notifierData.configurationWidget->saveNotifyConfigurations();

		foreach (const QString &eventKey, notifierData.events.keys())
			config_file.writeEntry("Notify", eventKey + '_' + key, notifierData.events[eventKey]);
	}
}

void NotifyConfigurationUiHandler::mainConfigurationWindowDestroyed()
{
	notificationsGroupBox = 0;

	QMap<QString, NotificationManager::NotifierData>::iterator notifierData;
	for (notifierData = NotificationManager::instance()->Notifiers.begin();
			notifierData != NotificationManager::instance()->Notifiers.end(); ++notifierData)
		(*notifierData).configurationWidget = 0;
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

void NotifyConfigurationUiHandler::eventSwitched(int index)
{
	kdebugf();

	CurrentEvent = notifications->currentItemValue();
	foreach(const QString &key, NotificationManager::instance()->Notifiers.keys())
	{
		NotificationManager::NotifierData notifierData = NotificationManager::instance()->Notifiers[key];

		if (!notifierData.events.contains(CurrentEvent))
			notifierData.events[CurrentEvent] = config_file.readBoolEntry("Notify", CurrentEvent + '_' + key);

		if (notifierData.configurationWidget)
			notifierData.configurationWidget->switchToEvent(CurrentEvent);

		if (notifierData.configurationGroupBox)
			notifierData.configurationGroupBox->setChecked(notifierData.events[CurrentEvent]);
	}
}

void NotifyConfigurationUiHandler::notifierToggled(const QString &notifier, bool toggled)
{
	kdebugf();

	NotificationManager::instance()->Notifiers[notifier].events[CurrentEvent] = toggled;
}
