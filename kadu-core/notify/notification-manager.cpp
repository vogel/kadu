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

#include "accounts/account.h"
#include "accounts/account_manager.h"

#include "chat/chat_manager.h"

#include "contacts/contact-account-data.h"
#include "contacts/contact-list.h"
#include "contacts/contact-manager.h"

#include "gui/widgets/chat_widget.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/widgets/configuration/config-combo-box.h"
#include "gui/widgets/configuration/config-group-box.h"
#include "gui/widgets/configuration/notify-group-box.h"
#include "gui/widgets/configuration/notifier-configuration-widget.h"
#include "gui/widgets/contacts-list-widget-menu-manager.h"
#include "gui/widgets/custom_input.h"

#include "gui/windows/configuration-window.h"

#include "action.h"
#include "config_file.h"
#include "debug.h"
#include "kadu_main_window.h"
#include "message_box.h"
#include "misc/misc.h"

#include "connection_error_notification.h"
#include "contact-notify-data.h"
#include "new_message_notification.h"
#include "status_changed_notification.h"

extern "C" KADU_EXPORT int notify_init(bool firstLoad)
{
	kdebugf();

	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/notify.ui"), NotificationManager::instance());

	kdebugf2();
	return 0;
}

extern "C" KADU_EXPORT void notify_close()
{
	kdebugf();

	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/notify.ui"), NotificationManager::instance());

	kdebugf2();
}

NotificationManager *NotificationManager::Instance = 0;

NotificationManager * NotificationManager::instance()
{
	if (!Instance)
	{
		Instance = new NotificationManager();

		MessageNotification::registerEvents();
		ConnectionErrorNotification::registerEvent();
		StatusChangedNotification::registerEvents();
	}

	return Instance;
}

NotificationManager::NotificationManager()
	: notificationsGroupBox(0)
{
	kdebugf();

	createDefaultConfiguration();

	triggerAllAccountsRegistered();

	notifyAboutUserActionDescription = new ActionDescription(0,
		ActionDescription::TypeUser, "notifyAboutUserAction",
		this, SLOT(notifyAboutUserActionActivated(QAction *, bool)),
		"NotifyAboutUser", tr("Notify about user"), true, "",
		checkNotify
	);

	ContactsListWidgetMenuManager::instance()->addManagementActionDescription(notifyAboutUserActionDescription);

	kdebugf2();
}

NotificationManager::~NotificationManager()
{
	kdebugf();

	ContactsListWidgetMenuManager::instance()->removeManagementActionDescription(notifyAboutUserActionDescription);
	delete notifyAboutUserActionDescription;

	StatusChangedNotification::unregisterEvents();
	ConnectionErrorNotification::unregisterEvent();
	MessageNotification::unregisterEvents();

	triggerAllAccountsUnregistered();

	if (!Notifiers.isEmpty())
	{
		kdebugm(KDEBUG_WARNING, "WARNING: not unregistered notifiers found! (%u)\n", Notifiers.size());
		QStringList notifierNames = Notifiers.keys();
		foreach(const QString &name, notifierNames)
			unregisterNotifier(name);
	}

	kdebugf2();
}

void NotificationManager::notifyAboutUserActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
		return;

	ContactList contacts = window->contacts();

	bool on = true;
	foreach (const Contact contact, contacts)
	{
		ContactNotifyData *cnd = contact.moduleData<ContactNotifyData>();

		if (!cnd || !cnd->notify())
		{
			on = false;
			break;
		}

		delete cnd;
	}

	foreach (const Contact contact, contacts)
	{
		if (contact.isNull() || contact.isAnonymous())
			continue;

		ContactNotifyData *cnd = contact.moduleData<ContactNotifyData>();
		if (!cnd)
			continue;

		if (cnd->notify() == on)
		{
			cnd->setNotify(!on);
			cnd->storeConfiguration();
			delete cnd;
		}
	}

	foreach(KaduAction *action, notifyAboutUserActionDescription->actions())
	{
		if (action->contacts() == contacts)
			action->setChecked(!on);
	}

	kdebugf2();
}

void NotificationManager::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	connect(mainConfigurationWindow, SIGNAL(destroyed()), this, SLOT(mainConfigurationWindowDestroyed()));

	notifications = dynamic_cast<ConfigComboBox *>(mainConfigurationWindow->widget()->widgetById("notify/notifications"));
	connect(notifications, SIGNAL(activated(int)), this, SLOT(eventSwitched(int)));

	QStringList captions;
	QStringList values;

	foreach(const NotifyEvent &notifyEvent, NotifyEvents)
	{
		captions.append(qApp->translate("@default", notifyEvent.description));
		values.append(notifyEvent.name);
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

	foreach(const QString &key, Notifiers.keys())
		addConfigurationWidget(Notifiers[key], key);

	eventSwitched(0);
}

void NotificationManager::mainConfigurationWindowDestroyed()
{
	notificationsGroupBox = 0;

	QMap<QString, NotifierData>::iterator notifierData;
	for (notifierData = Notifiers.begin(); notifierData != Notifiers.end(); ++notifierData)
		(*notifierData).configurationWidget = 0;
}

void NotificationManager::addConfigurationWidget(NotifierData &notifier, const QString &name)
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

void NotificationManager::removeConfigurationWidget(NotifierData &notifier)
{
	if (notifier.configurationWidget)
	{
		delete notifier.configurationWidget;
		notifier.configurationWidget = 0;
	}

	delete notifier.configurationGroupBox;
	notifier.configurationGroupBox = 0;
}

void NotificationManager::eventSwitched(int index)
{
	kdebugf();

	CurrentEvent = notifications->currentItemValue();
	foreach(const QString &key, Notifiers.keys())
	{
		NotifierData notifierData = Notifiers[key];

		if (!notifierData.events.contains(CurrentEvent))
			notifierData.events[CurrentEvent] = config_file.readBoolEntry("Notify", CurrentEvent + '_' + key);

		if (notifierData.configurationWidget)
			notifierData.configurationWidget->switchToEvent(CurrentEvent);

		if (notifierData.configurationGroupBox)
			notifierData.configurationGroupBox->setChecked(notifierData.events[CurrentEvent]);
	}
}

void NotificationManager::configurationWindowApplied()
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

	foreach(const QString &key, Notifiers.keys())
	{
		NotifierData notifierData = Notifiers[key];

		if (notifierData.configurationWidget)
			notifierData.configurationWidget->saveNotifyConfigurations();

		foreach(const QString &eventKey, notifierData.events.keys())
			config_file.writeEntry("Notify", eventKey + '_' + key, notifierData.events[eventKey]);
	}
}

void NotificationManager::notifierToggled(const QString &notifier, bool toggled)
{
	kdebugf();

	Notifiers[notifier].events[CurrentEvent] = toggled;
}

void NotificationManager::moveToAllList()
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

void NotificationManager::moveToNotifyList()
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

void NotificationManager::accountRegistered(Account *account)
{
	Protocol *protocol = account->protocol();
	connect(protocol, SIGNAL(connectionError(Account *, const QString &, const QString &)),
			this, SLOT(connectionError(Account *, const QString &, const QString &)));
	connect(protocol, SIGNAL(messageReceived(Account *, ContactList, const QString&, time_t)),
		this, SLOT(messageReceived(Account *, ContactList, const QString&, time_t)));
	connect(account, SIGNAL(contactStatusChanged(Account *, Contact, Status)),
		this, SLOT(statusChanged(Account *, Contact, Status)));
}

void NotificationManager::accountUnregistered(Account *account)
{
	Protocol *protocol = account->protocol();
	disconnect(protocol, SIGNAL(connectionError(Account *, const QString &, const QString &)),
			this, SLOT(connectionError(Account *, const QString &, const QString &)));
	disconnect(protocol, SIGNAL(messageReceived(Account *, ContactList, const QString&, time_t)),
		this, SLOT(messageReceived(Account *, ContactList, const QString&, time_t)));
	disconnect(account, SIGNAL(contactStatusChanged(Account *, Contact, Status)),
		this, SLOT(statusChanged(Account *, Contact, Status)));
}

void NotificationManager::statusChanged(Account *account, Contact contact, Status oldStatus)
{
	kdebugf();

	// TODO 0.6.6
	/*if (massively && config_file.readBoolEntry("Notify", "NotifyIgnoreOnConnection"))
	{
		kdebugmf(KDEBUG_FUNCTION_END, "end: ignore on connection\n");
		return;
	}*/

	// TODO 0.6.6 display -> uuid?
	bool notify_contact = true;
	ContactNotifyData *cnd = contact.moduleData<ContactNotifyData>();

	if (!cnd || !cnd->notify())
		notify_contact = false;

	delete cnd;

	if (!notify_contact && !config_file.readBoolEntry("Notify", "NotifyAboutAll"))
	{
		kdebugmf(KDEBUG_FUNCTION_END, "end: not notifying user AND not notifying all users\n");
		return;
	}

	if (contact.id(account) == account->id())
		return;

	ContactAccountData *data = contact.accountData(account);
	if (!data)
		return;

	if (config_file.readBoolEntry("Notify", "IgnoreOnlineToOnline") &&
		(data->status().isOnline() || data->status().isBusy()) &&
			(oldStatus.isOnline() || oldStatus.isBusy()))
				return;

	QString changedTo = "To" + Status::name(data->status(), false);

	ContactList contacts(contact);

	StatusChangedNotification *statusChangedNotification = new StatusChangedNotification(changedTo, contacts, account);
	notify(statusChangedNotification);

	kdebugf2();
}

void NotificationManager::messageReceived(Account *account, ContactList contacts, const QString &msg, time_t t)
{
	kdebugf();

	ChatWidget *chat = chat_manager->findChatWidget(contacts);
	if (!chat) // new chat
		notify(new MessageNotification(MessageNotification::NewChat, contacts, msg, account));
	else // new message in chat
		if (!chat->edit()->hasFocus() || !config_file.readBoolEntry("Notify", "NewMessageOnlyIfInactive"))
			notify(new MessageNotification(MessageNotification::NewMessage, contacts, msg, account));

	kdebugf2();
}

void NotificationManager::connectionError(Account *account, const QString &server, const QString &message)
{
	kdebugf();

	if (!ConnectionErrorNotification::activeError(message))
	{
		ConnectionErrorNotification *connectionErrorNotification = new ConnectionErrorNotification(server, message, account);
		notify(connectionErrorNotification);
	}

	kdebugf2();
}

void NotificationManager::registerEvent(const QString &name, const char *description, CallbackRequirement callbackRequirement)
{
	kdebugf();

	NotifyEvent event;
	event.name = name;
	event.description = description;
	event.callbackRequirement = callbackRequirement;

	NotifyEvents.append(event);

	kdebugf2();
}

void NotificationManager::unregisterEvent(const QString &name)
{
	kdebugf();

	NotifyEvent remove;
	remove.name = name;
	NotifyEvents.removeAll(remove);

	kdebugf2();
}

void NotificationManager::registerNotifier(const QString &name, Notifier *notifier)
{
	kdebugf();
	if (Notifiers.contains(name))
	{
		kdebugm(KDEBUG_WARNING, "WARNING: '%s' already exists in notifiers! "
		"strange... unregistering old Notifier\n", qPrintable(name));

		unregisterNotifier(name);
	}

	// TODO: remove after 0.6 release
	if (config_file.readBoolEntry("Notify", "StatusChanged_" + name, false))
	{
		QStringList addToMe;
		addToMe << "toAvailable" << "toBusy" << "toInvisible" << "toNotAvailable";

		foreach(const QString &i, addToMe)
		{
			if (!config_file.readBoolEntry("Notify", i + '_' + name))
			{
				notifier->copyConfiguration("StatusChanged", i);
				config_file.writeEntry("Notify", i + '_' + name, true);
			}
		}

		config_file.removeVariable("Notify", "StatusChanged_" + name);
	}

	Notifiers[name].notifier = notifier;
	Notifiers[name].configurationWidget = 0;
	Notifiers[name].configurationGroupBox = 0;

	if (notificationsGroupBox)
		addConfigurationWidget(Notifiers[name], name);

	kdebugf2();
}

void NotificationManager::unregisterNotifier(const QString &name)
{
	kdebugf();

	if (!Notifiers.contains(name))
	{
		kdebugm(KDEBUG_WARNING, "WARNING: '%s' not registered!\n", qPrintable(name));
		return;
	}

	if (notificationsGroupBox)
		removeConfigurationWidget(Notifiers[name]);

	Notifiers.remove(name);

	kdebugf2();
}

QStringList NotificationManager::notifiersList() const
{
	return QStringList(Notifiers.keys());
}

const QList<NotificationManager::NotifyEvent> &NotificationManager::notifyEvents()
{
	return NotifyEvents;
}

void NotificationManager::notify(Notification *notification)
{
	kdebugf();

	QString notifyType = notification->type();
	bool foundNotifier = false;
	bool foundNotifierWithCallbackSupported = notification->getCallbacks().count() == 0;

	notification->acquire();

	foreach(const QString &key, Notifiers.keys())
	{
		NotifierData notifier = Notifiers[key];

		if (config_file.readBoolEntry("Notify", notifyType + '_' + key))
		{
			notifier.notifier->notify(notification);
			foundNotifier = true;
			foundNotifierWithCallbackSupported = foundNotifierWithCallbackSupported || (notifier.notifier->callbackCapacity() == CallbackSupported);
		}
	}

	if (!foundNotifierWithCallbackSupported)
		foreach(const QString &key, Notifiers.keys())
		{
			NotifierData notifier = Notifiers[key];

			if (notifier.notifier->callbackCapacity() == CallbackSupported)
			{
				notifier.notifier->notify(notification);
				foundNotifier = true;
				foundNotifierWithCallbackSupported = true;
				break;
			}
		}

	if (!foundNotifier)
		notification->callbackDiscard();

	notification->release();

	if (!foundNotifierWithCallbackSupported)
		MessageBox::msg(tr("Unable to find notifier for %1 event").arg(notification->type()), true, "Warning");

	kdebugf2();
}

void NotificationManager::createDefaultConfiguration()
{
	config_file.addVariable("Notify", "IgnoreOnlineToOnline", false);
	config_file.addVariable("Notify", "NewMessageOnlyIfInactive", true);
	config_file.addVariable("Notify", "NotifyAboutAll", true);
	config_file.addVariable("Notify", "NotifyIgnoreOnConnection", true);
}

void checkNotify(KaduAction *action)
{
	kdebugf();

	if (config_file.readBoolEntry("Notify", "NotifyAboutAll"))
	{
		action->setEnabled(false);
		return;
	}

	foreach(Contact contact, action->contacts())
		if (!contact.hasAccountData(contact.prefferedAccount()))
		{
			action->setEnabled(false);
			return;
		}

	action->setEnabled(true);

	bool on = true;
	foreach (const Contact contact, action->contacts())
	{
		ContactNotifyData *cnd = contact.moduleData<ContactNotifyData>();

		if (!cnd || !cnd->notify())
		{
			on = false;
			break;
		}

		delete cnd;
	}

	action->setChecked(on);

	kdebugf2();
}
