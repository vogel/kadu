/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "core/injected-factory.h"
#include "widgets/chat-widget/chat-widget.h"
#include "widgets/configuration/config-combo-box.h"
#include "widgets/configuration/config-group-box.h"
#include "widgets/configuration/configuration-widget.h"
#include "widgets/configuration/notifier-configuration-widget.h"
#include "widgets/configuration/notify-group-box.h"
#include "widgets/configuration/notify-tree-widget.h"
#include "windows/configuration-window.h"

#include "notification-event-repository.h"
#include "notification-event.h"
#include "notifier-repository.h"
#include "notifier.h"

#include "notify-configuration-ui-handler.h"

NotifyConfigurationUiHandler::NotifyConfigurationUiHandler(QObject *parent)
        : QObject{parent}, allUsers{}, notifiedUsers{}, notificationsGroupBox{}, useCustomSettingsCheckBox{},
          notifyTreeWidget{}
{
}

NotifyConfigurationUiHandler::~NotifyConfigurationUiHandler()
{
}

void NotifyConfigurationUiHandler::setConfiguration(Configuration *configuration)
{
    m_configuration = configuration;
}

void NotifyConfigurationUiHandler::setInjectedFactory(InjectedFactory *injectedFactory)
{
    m_injectedFactory = injectedFactory;
}

void NotifyConfigurationUiHandler::setNotificationEventRepository(
    NotificationEventRepository *notificationEventRepository)
{
    m_notificationEventRepository = notificationEventRepository;
}

void NotifyConfigurationUiHandler::setNotifierRepository(NotifierRepository *notifierRepository)
{
    m_notifierRepository = notifierRepository;
}

void NotifyConfigurationUiHandler::init()
{
    connect(m_notifierRepository, SIGNAL(notifierRegistered(Notifier *)), this, SLOT(notifierRegistered(Notifier *)));
    connect(
        m_notifierRepository, SIGNAL(notifierUnregistered(Notifier *)), this, SLOT(notifierUnregistered(Notifier *)));

    connect(
        m_notificationEventRepository, SIGNAL(notificationEventAdded(NotificationEvent)), this,
        SLOT(notificationEventAdded(NotificationEvent)));
    connect(
        m_notificationEventRepository, SIGNAL(notificationEventRemoved(NotificationEvent)), this,
        SLOT(notificationEventRemoved(NotificationEvent)));
}

void NotifyConfigurationUiHandler::addConfigurationWidget(Notifier *notifier)
{
    NotifyGroupBox *configurationGroupBox = m_injectedFactory->makeInjected<NotifyGroupBox>(
        notifier, QCoreApplication::translate("@default", notifier->description().toUtf8().constData()),
        notificationsGroupBox->widget());
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

    notifierMainWidgetLayout->addWidget(configurationGroupBox);
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
    for (auto notifier : m_notifierRepository)
    {
        for (auto &&notifyEvent : m_notificationEventRepository->notificationEvents())
        {
            if (!NotifierGui[notifier].Events.contains(notifyEvent.name()))
                NotifierGui[notifier].Events[notifyEvent.name()] = m_configuration->deprecatedApi()->readBoolEntry(
                    "Notify", notifyEvent.name() + '_' + notifier->name());
        }
    }

    QString eventName;
    for (auto &&notifyEvent : m_notificationEventRepository->notificationEvents())
    {
        eventName = notifyEvent.name();
        if (NotificationEvents.contains(eventName))
            continue;

        NotificationEventConfigurationItem item;
        item.event = notifyEvent;
        item.useCustomSettings =
            m_configuration->deprecatedApi()->readBoolEntry("Notify", eventName + "_UseCustomSettings", false);

        NotificationEvents[eventName] = item;
    }

    notificationsGroupBox =
        mainConfigurationWindow->widget()->configGroupBox("Notifications", "Events", "Notifications");

    notifierMainWidget = new QWidget{notificationsGroupBox->widget()};
    notifierMainWidgetLayout = new QVBoxLayout(notifierMainWidget);
    notifierMainWidgetLayout->setMargin(0);

    notificationsGroupBox->addWidget(notifierMainWidget, true);

    notifyTreeWidget = m_injectedFactory->makeInjected<NotifyTreeWidget>(this, notificationsGroupBox->widget());
    notifyTreeWidget->setMinimumHeight(300);
    notifierMainWidgetLayout->addWidget(notifyTreeWidget, 100);
    notifyTreeWidget->setCurrentItem(notifyTreeWidget->topLevelItem(0));
    connect(notifyTreeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(eventSwitched()));

    useCustomSettingsCheckBox = new QCheckBox(tr("Use custom settings"));
    connect(useCustomSettingsCheckBox, SIGNAL(toggled(bool)), this, SLOT(customSettingsCheckBoxToggled(bool)));
    notifierMainWidgetLayout->addWidget(useCustomSettingsCheckBox);

    for (auto notifier : m_notifierRepository)
        addConfigurationWidget(notifier);

    eventSwitched();
}

void NotifyConfigurationUiHandler::mainConfigurationWindowDestroyed()
{
    notificationsGroupBox = 0;
    NotifierGui.clear();
}

void NotifyConfigurationUiHandler::mainConfigurationWindowApplied()
{
    for (auto &&notifyEvent : m_notificationEventRepository->notificationEvents())
    {
        if (notifyEvent.category().isEmpty() || !NotificationEvents.contains(notifyEvent.name()))
            continue;

        m_configuration->deprecatedApi()->writeEntry(
            "Notify", notifyEvent.name() + "_UseCustomSettings",
            NotificationEvents[notifyEvent.name()].useCustomSettings);
    }

    for (auto notifier : m_notifierRepository)
    {
        if (!NotifierGui.contains(notifier))
            continue;

        NotifierConfigurationGuiItem &gui = NotifierGui[notifier];
        if (gui.ConfigurationWidget)
            gui.ConfigurationWidget->saveNotifyConfigurations();

        for (QMap<QString, bool>::const_iterator it = gui.Events.constBegin(), end = gui.Events.constEnd(); it != end;
             ++it)
            m_configuration->deprecatedApi()->writeEntry("Notify", it.key() + '_' + notifier->name(), it.value());
    }
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

void NotifyConfigurationUiHandler::notificationEventAdded(NotificationEvent notifyEvent)
{
    if (notificationsGroupBox)
    {
        QString eventName = notifyEvent.name();
        NotificationEventConfigurationItem item;
        item.event = notifyEvent;
        if (!notifyEvent.category().isEmpty())
            item.useCustomSettings =
                m_configuration->deprecatedApi()->readBoolEntry("Notify", eventName + "_UseCustomSettings", false);
        else
            item.useCustomSettings = true;

        NotificationEvents[eventName] = item;

        notifyTreeWidget->refresh();
    }
}

void NotifyConfigurationUiHandler::notificationEventRemoved(NotificationEvent notifyEvent)
{
    auto eventName = notifyEvent.name();
    if (NotificationEvents.contains(eventName))
        NotificationEvents.remove(eventName);

    if (notificationsGroupBox)
        notifyTreeWidget->refresh();
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
    CurrentEvent = notifyTreeWidget->currentEvent();

    useCustomSettingsCheckBox->setVisible(!NotificationEvents[CurrentEvent].event.category().isEmpty());
    useCustomSettingsCheckBox->setChecked(NotificationEvents[CurrentEvent].useCustomSettings);
    customSettingsCheckBoxToggled(
        useCustomSettingsCheckBox->isHidden() || NotificationEvents[CurrentEvent].useCustomSettings);

    for (auto notifier : m_notifierRepository)
    {
        if (!NotifierGui.contains(notifier))
            NotifierGui.insert(notifier, NotifierConfigurationGuiItem());

        NotifierConfigurationGuiItem &gui = NotifierGui[notifier];

        if (!gui.Events.contains(CurrentEvent))
            gui.Events[CurrentEvent] =
                m_configuration->deprecatedApi()->readBoolEntry("Notify", CurrentEvent + '_' + notifier->name());

        if (gui.ConfigurationWidget)
            gui.ConfigurationWidget->switchToEvent(CurrentEvent);

        if (gui.ConfigurationGroupBox)
            gui.ConfigurationGroupBox->setChecked(gui.Events[CurrentEvent]);
    }
}

void NotifyConfigurationUiHandler::notifierToggled(Notifier *notifier, bool toggled)
{
    if (!NotifierGui.contains(notifier))
        NotifierGui.insert(notifier, NotifierConfigurationGuiItem());
    NotifierGui[notifier].Events[CurrentEvent] = toggled;

    notifyTreeWidget->notifierChecked(notifier, toggled);
}

void NotifyConfigurationUiHandler::customSettingsCheckBoxToggled(bool toggled)
{
    NotificationEvents[CurrentEvent].useCustomSettings = toggled;

    notifyTreeWidget->useCustomSettingsChecked(toggled);

    foreach (const NotifierConfigurationGuiItem &guiItem, NotifierGui)
        if (guiItem.ConfigurationGroupBox)
            guiItem.ConfigurationGroupBox->setVisible(toggled);
}

#include "moc_notify-configuration-ui-handler.cpp"
