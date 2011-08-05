/*
 * %kadu copyright begin%
 * Copyright 2011 Marcin Dawidziuk (cinekdawidziuk@gmail.com)
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

#include <QtCore/QPoint>
#include <QtGui/QImage>
#include <QtGui/QMouseEvent>

#include <libindicate-qt/qindicateserver.h>

#include "avatars/avatar.h"
#include "buddies/buddy.h"
#include "chat/chat.h"
#include "chat/chat-manager.h"
#include "configuration/configuration-file.h"
#include "contacts/contact.h"
#include "contacts/contact-set.h"
#include "core/core.h"
#include "gui/widgets/chat-widget-manager.h"
#include "misc/path-conversion.h"
#include "notify/chat-notification.h"
#include "notify/notification-manager.h"

#include "plugins/docking/docking.h"

#include "indicator_docking.h"

IndicatorDocking * IndicatorDocking::Instance = 0;

void IndicatorDocking::createInstance()
{
	if (!Instance)
		Instance = new IndicatorDocking();
}

void IndicatorDocking::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

IndicatorDocking * IndicatorDocking::instance()
{ 
	return Instance;
}

IndicatorDocking::IndicatorDocking() :
		Notifier("IndicatorNotify", QT_TRANSLATE_NOOP("@default", "Indicator"), KaduIcon("external_modules/mail-internet-mail")),
		EventForShowMainWindow(new QMouseEvent(QEvent::MouseButtonPress, QPoint(0, 0), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier))
{
	Server = QIndicate::Server::defaultInstance();
	Server->setDesktopFile(desktopFilePath());
	Server->setType("message.im");
	Server->show();

	connect(Server, SIGNAL(serverDisplay()), this, SLOT(showMainWindow()));
	connect(ChatWidgetManager::instance(), SIGNAL(chatWidgetActivated(ChatWidget *)), this, SLOT(chatWidgetActivated()));

	createDefaultConfiguration();

	DockingManager::instance()->setDocker(this);
	NotificationManager::instance()->registerNotifier(this);
}

IndicatorDocking::~IndicatorDocking()
{
	NotificationManager::instance()->unregisterNotifier(this);
	DockingManager::instance()->setDocker(0);

	disconnect(Server, SIGNAL(serverDisplay()), this, SLOT(showMainWindow()));
	disconnect(ChatWidgetManager::instance(), SIGNAL(chatWidgetActivated(ChatWidget *)), this, SLOT(chatWidgetActivated()));

	deleteAllIndicators();
}

void IndicatorDocking::showMainWindow()
{
	DockingManager::instance()->trayMousePressEvent(EventForShowMainWindow.data());
}

void IndicatorDocking::notify(Notification *notification)
{
	if (notification->type() != "NewMessage" && notification->type() != "NewChat")
		return;

	notification->clearDefaultCallback();
	notification->acquire();

	ChatNotification *chatNotification = qobject_cast<ChatNotification *>(notification);
	if (!chatNotification)
		return;

	Chat chat = chatNotification->chat();
	Contact contact = *chat.contacts().constBegin();
	QString buddyName = contact.ownerBuddy().display();
	if (buddyName.isEmpty())
		return;

	QIndicate::Indicator *indicator;
	if (IndicatorsMap.contains(buddyName))
	{
		indicator = IndicatorsMap[buddyName];
		ChatsMap[buddyName] = chat;
	}
	else
	{
		indicator = new QIndicate::Indicator(Server);
		IndicatorsMap[buddyName] = indicator;
		ChatsMap[buddyName] = chat;
		ContactsMap[buddyName] = chat.contacts();
		indicator->setNameProperty(buddyName);

		Avatar avatar = contact.contactAvatar();
		AvatarsMap[buddyName] = QImage(avatar.pixmap().toImage().scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation));
		indicator->setIconProperty(AvatarsMap[buddyName]);

		connect(indicator, SIGNAL(display(QIndicate::Indicator*)), SLOT(displayIndicator(QIndicate::Indicator*)));
	}

	connect(notification, SIGNAL(closed(Notification*)), this, SLOT(notificationClosed(Notification*)));
	indicator->setTimeProperty(QDateTime::currentDateTime());
	indicator->setDrawAttentionProperty(true);
	indicator->show();
	IndicatorsVisible[buddyName] = true;
}

void IndicatorDocking::notificationClosed(Notification *notification)
{
	ChatNotification *chatNotification = qobject_cast<ChatNotification *>(notification);
	if (!chatNotification)
		return;

	Contact contact = *chatNotification->chat().contacts().begin();
	QString buddyName = contact.ownerBuddy().display();
	if (buddyName.isEmpty())
		return;

	deleteIndicator(buddyName);
}

void IndicatorDocking::chatWidgetActivated()
{
	deleteAllIndicators();
}

void IndicatorDocking::displayIndicator(QIndicate::Indicator *indicator)
{
	QString indicatorName = indicator->nameProperty();
	if (!ChatsMap.contains(indicatorName))
		return;

	ContactSet contacts = ContactsMap[indicatorName];
	Chat chat = ChatManager::instance()->findChat(contacts);
	ChatWidgetManager::instance()->openPendingMessages(chat);
	//Don't have to deleteIndicator when you call chatWidgetActivated
}

void IndicatorDocking::deleteIndicator(const QString &name)
{
	if (IndicatorsVisible.contains(name) && IndicatorsVisible[name] && IndicatorsMap.contains(name))
	{
		IndicatorsVisible[name] = false;
		IndicatorsMap[name]->hide();
	}
}

void IndicatorDocking::deleteAllIndicators()
{
	qDeleteAll(IndicatorsMap);
	IndicatorsMap.clear();
}

void IndicatorDocking::createDefaultConfiguration()
{
	config_file.addVariable("Notify", "NewChat_IndicatorNotify", true);
	config_file.addVariable("Notify", "NewMessage_IndicatorNotify", true);
}
