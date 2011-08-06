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
#include <QtCore/QSet>
#include <QtGui/QImage>
#include <QtGui/QMouseEvent>

#include <libindicate-qt/qindicateserver.h>

#include "avatars/avatar.h"
#include "buddies/buddy.h"
#include "chat/aggregate-chat-manager.h"
#include "chat/chat.h"
#include "chat/chat-details-aggregate.h"
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
		EventForShowMainWindow(new QMouseEvent(QEvent::MouseButtonPress, QPoint(0, 0), Qt::LeftButton, Qt::LeftButton, Qt::ControlModifier))
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

	ChatNotification *chatNotification = qobject_cast<ChatNotification *>(notification);
	if (!chatNotification)
		return;

	Chat chat = chatNotification->chat();
	if (!chat)
		return;

	chatNotification->clearDefaultCallback();
	chatNotification->acquire();

	// First we need to search for exactly the same chat.
	QIndicate::Indicator *indicator = 0;
	QMultiMap<QIndicate::Indicator *, ChatNotification *>::iterator it = IndicatorsMap.begin();
	QMultiMap<QIndicate::Indicator *, ChatNotification *>::iterator end = IndicatorsMap.end();
	for (; it != end; ++it)
		if (it.value()->chat() == chat)
		{
			disconnect(it.value(), SIGNAL(closed(Notification*)), this, SLOT(notificationClosed(Notification*)));
			it.value()->release();
			it.value() = chatNotification;
			indicator = it.key();
			break;
		}

	if (!indicator)
	{
		// Now, if we didn't find the same chat, we need to check if it is a message from a contact of a buddy we already have.
		// TODO: It should be somehow supported by core. Currently this API is way too hard to use.

		Chat aggregateChat = AggregateChatManager::instance()->aggregateChat(chat);
		ChatDetailsAggregate *aggregateChatDetails = qobject_cast<ChatDetailsAggregate *>(aggregateChat.details());
		if (aggregateChatDetails)
		{
			QMultiMap<QIndicate::Indicator *, ChatNotification *>::const_iterator it = IndicatorsMap.constBegin();
			QMultiMap<QIndicate::Indicator *, ChatNotification *>::const_iterator end = IndicatorsMap.constEnd();
			for (; it != end; ++it)
				if (aggregateChatDetails->chats().contains(it.value()->chat()))
				{
					indicator = it.key();
					break;
				}
		}

		if (!indicator)
		{
			Contact firstContact = *chat.contacts().constBegin();

			indicator = new QIndicate::Indicator(this);
			IndicatorsMap.insert(indicator, chatNotification);
			indicator->setNameProperty(firstContact.ownerBuddy().display());

			Avatar avatar = firstContact.contactAvatar();
			if (avatar && !avatar.pixmap().isNull())
				indicator->setIconProperty(avatar.pixmap().toImage().scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation));

			connect(indicator, SIGNAL(display(QIndicate::Indicator*)), SLOT(displayIndicator(QIndicate::Indicator*)));
		}
		else
		{
			IndicatorsMap.insertMulti(indicator, chatNotification);

			Avatar avatar = chat.contacts().constBegin()->contactAvatar();
			if (avatar && !avatar.pixmap().isNull())
				indicator->setIconProperty(avatar.pixmap().toImage().scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation));
		}
	}

	connect(chatNotification, SIGNAL(closed(Notification*)), this, SLOT(notificationClosed(Notification*)));
	indicator->setTimeProperty(QDateTime::currentDateTime());
	indicator->setDrawAttentionProperty(true);
	indicator->show();
}

void IndicatorDocking::notificationClosed(Notification *notification)
{
	ChatNotification *chatNotification = qobject_cast<ChatNotification *>(notification);
	if (!chatNotification)
		return;

	deleteIndicator(chatNotification);
}

void IndicatorDocking::chatWidgetActivated()
{
	deleteAllIndicators();
}

void IndicatorDocking::displayIndicator(QIndicate::Indicator *indicator)
{
	// In case we have multiple chats for that indicator, the most recently inserted one will be opened.
	ChatNotification *chatNotification = IndicatorsMap.value(indicator);
	if (!chatNotification)
		return;

	ChatWidgetManager::instance()->openPendingMessages(chatNotification->chat(), true);
	//Don't have to deleteIndicator when you call chatWidgetActivated
}

void IndicatorDocking::deleteIndicator(ChatNotification *chatNotification)
{
	QIndicate::Indicator *indicator = 0;
	QMultiMap<QIndicate::Indicator *, ChatNotification *>::iterator it = IndicatorsMap.begin();
	QMultiMap<QIndicate::Indicator *, ChatNotification *>::iterator end = IndicatorsMap.end();
	for (; it != end; ++it)
		if (it.value() == chatNotification)
		{
			indicator = it.key();
			IndicatorsMap.erase(it);
			break;
		}

	if (!indicator)
		return;

	if (!IndicatorsMap.contains(indicator))
		indicator->hide();
}

void IndicatorDocking::deleteAllIndicators()
{
	QSet<QIndicate::Indicator *> indicatorsToDelete;
	QMultiMap<QIndicate::Indicator *, ChatNotification *>::const_iterator it = IndicatorsMap.constBegin();
	QMultiMap<QIndicate::Indicator *, ChatNotification *>::const_iterator end = IndicatorsMap.constEnd();
	for (; it != end; ++it)
	{
		disconnect(it.value(), SIGNAL(closed(Notification*)), this, SLOT(notificationClosed(Notification*)));
		it.value()->release();
		// because it is a multimap, keys may repeat
		indicatorsToDelete.insert(it.key());
	}

	IndicatorsMap.clear();
	qDeleteAll(indicatorsToDelete);
}

void IndicatorDocking::createDefaultConfiguration()
{
	config_file.addVariable("Notify", "NewChat_IndicatorNotify", true);
	config_file.addVariable("Notify", "NewMessage_IndicatorNotify", true);
}
