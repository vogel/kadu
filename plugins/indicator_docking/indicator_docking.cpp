/*
 * %kadu copyright begin%
 * Copyright 2011 Marcin Dawidziuk (cinekdawidziuk@gmail.com)
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QList>
#include <QtCore/QPoint>
#include <QtCore/QSet>
#include <QtCore/QTimer>
#include <QtGui/QImage>
#include <QtGui/QMouseEvent>

#include <libindicate-qt/qindicateserver.h>

#include "avatars/avatar.h"
#include "buddies/buddy.h"
#include "chat/aggregate-chat-manager.h"
#include "chat/chat-details-aggregate.h"
#include "chat/chat-manager.h"
#include "chat/chat.h"
#include "chat/model/chat-data-extractor.h"
#include "configuration/configuration-file.h"
#include "contacts/contact-set.h"
#include "contacts/contact.h"
#include "core/core.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/widgets/chat-widget.h"
#include "message/message-manager.h"
#include "misc/path-conversion.h"
#include "notify/new-message-notification.h"
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
	connect(ChatManager::instance(), SIGNAL(chatUpdated(Chat)), this, SLOT(chatUpdated(Chat)));
	connect(ChatWidgetManager::instance(), SIGNAL(chatWidgetCreated(ChatWidget*)), this, SLOT(chatWidgetCreated(ChatWidget*)));
	connect(NotificationManager::instance(), SIGNAL(silentModeToggled(bool)), this, SLOT(silentModeToggled(bool)));

	createDefaultConfiguration();

	DockingManager::instance()->setDocker(this);
	NotificationManager::instance()->registerNotifier(this);

	QTimer::singleShot(0, this, SLOT(indicateUnreadMessages()));
}

IndicatorDocking::~IndicatorDocking()
{
	NotificationManager::instance()->unregisterNotifier(this);
	DockingManager::instance()->setDocker(0);

	disconnect(Server, SIGNAL(serverDisplay()), this, SLOT(showMainWindow()));
	disconnect(ChatManager::instance(), SIGNAL(chatUpdated(Chat)), this, SLOT(chatUpdated(Chat)));
	disconnect(ChatWidgetManager::instance(), SIGNAL(chatWidgetCreated(ChatWidget*)), this, SLOT(chatWidgetCreated(ChatWidget*)));

	QSet<QIndicate::Indicator *> indicatorsToDelete;
	IndMMap::const_iterator end = IndicatorsMap.constEnd();
	for (IndMMap::const_iterator it = IndicatorsMap.constBegin(); it != end; ++it)
	{
		disconnect(it.value(), SIGNAL(closed(Notification*)), this, SLOT(notificationClosed(Notification*)));
		it.value()->release();
		// because it is a multimap, keys may repeat
		indicatorsToDelete.insert(it.key());
	}

	IndicatorsMap.clear();
	qDeleteAll(indicatorsToDelete);

	Server->hide();
}

void IndicatorDocking::indicateUnreadMessages()
{
	if (config_file.readBoolEntry("Notify", "NewChat_IndicatorNotify") && !NotificationManager::instance()->silentMode())
		foreach (const Message &message, MessageManager::instance()->allUnreadMessages())
			notify(new MessageNotification(MessageNotification::NewChat, message));
}

void IndicatorDocking::silentModeToggled(bool silentMode)
{
	foreach (QIndicate::Indicator *indicator, IndicatorsMap.uniqueKeys())
		indicator->setDrawAttentionProperty(!silentMode);

	if (!silentMode)
		indicateUnreadMessages();
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
	IndMMap::iterator it = iteratorForChat(chat);
	if (it != IndicatorsMap.end())
	{
		disconnect(it.value(), SIGNAL(closed(Notification*)), this, SLOT(notificationClosed(Notification*)));
		it.value()->release();
		it.value() = chatNotification;
		indicator = it.key();
	}
	else
	{
		// Now, if we didn't find the same chat, we need to check if it is a message from a contact of a buddy we already have.
		// TODO: It should be somehow supported by core. Currently this API is way too hard to use.

		QList<IndMMap::iterator> iterators = iteratorsForAggregateChat(chat);
		if (!iterators.isEmpty())
		{
			indicator = iterators.at(0).key();

			IndicatorsMap.insertMulti(indicator, chatNotification);

			if (chat.contacts().count() == 1)
			{
				const Avatar &avatar = chat.contacts().constBegin()->avatar(true);
				if (!avatar.isEmpty())
					indicator->setIconProperty(avatar.pixmap().toImage().scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation));
			}
		}
		else
		{
			indicator = new QIndicate::Indicator(this);
			IndicatorsMap.insert(indicator, chatNotification);

			indicator->setNameProperty(ChatDataExtractor::data(chat, Qt::DisplayRole).toString());
			if (chat.contacts().count() == 1)
			{
				Avatar avatar = chat.contacts().constBegin()->contactAvatar();
				if (avatar && !avatar.pixmap().isNull())
					indicator->setIconProperty(avatar.pixmap().toImage().scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation));
			}

			connect(indicator, SIGNAL(display(QIndicate::Indicator*)), SLOT(displayIndicator(QIndicate::Indicator*)));
		}
	}

	connect(chatNotification, SIGNAL(closed(Notification*)), this, SLOT(notificationClosed(Notification*)));
	// TODO: set correct time for pending messages as well
	indicator->setTimeProperty(QDateTime::currentDateTime());
	indicator->setDrawAttentionProperty(true);
	indicator->show();
}

void IndicatorDocking::notificationClosed(Notification *notification)
{
	ChatNotification *chatNotification = qobject_cast<ChatNotification *>(notification);
	if (!chatNotification)
		return;

	removeNotification(chatNotification);
}

void IndicatorDocking::chatUpdated(const Chat &chat)
{
	// When a chat widget is activated, it contains only messages from its own chat, not aggregate chat.
	if (!chat || chat.unreadMessagesCount() > 0)
		return;

	IndMMap::iterator it = iteratorForChat(chat);
	if (it != IndicatorsMap.end())
		removeNotification(it.value());
}

void IndicatorDocking::chatWidgetCreated(ChatWidget *chatWidget)
{
	// When a chat widget is created, it is filled with all messages from given aggregate chat.

	if (!chatWidget)
		return;

	Chat chat = chatWidget->chat();
	if (!chat)
		return;

	QList<IndMMap::iterator> iterators = iteratorsForAggregateChat(chat);

	// It can't be done this way in one pass because removeNotification() modifies IndicatorsMap's structure
	// and thus invalidates existing iterators.
	QList<ChatNotification *> notificationsToRemove;
	foreach (const IndMMap::iterator &it, iterators)
		notificationsToRemove.append(it.value());

	foreach (ChatNotification *chatNotification, notificationsToRemove)
		removeNotification(chatNotification);
}

void IndicatorDocking::displayIndicator(QIndicate::Indicator *indicator)
{
	// In case we have multiple chats for that indicator, the most recently inserted one will be opened.
	ChatNotification *chatNotification = IndicatorsMap.value(indicator);
	if (!chatNotification)
		return;

	chatNotification->openChat();

	// chatUpdated() or chatWidgetCreated() slot will take care of deleting indicator
}

void IndicatorDocking::removeNotification(ChatNotification *chatNotification)
{
	if (!chatNotification)
		return;

	IndMMap::iterator it = iteratorForChat(chatNotification->chat());
	if (it == IndicatorsMap.end())
		return;

	QIndicate::Indicator *indicator = it.key();
	disconnect(it.value(), SIGNAL(closed(Notification*)), this, SLOT(notificationClosed(Notification*)));
	it.value()->release();
	IndicatorsMap.erase(it);

	if (!IndicatorsMap.contains(indicator))
		delete indicator;
}

QMap<QIndicate::Indicator *, ChatNotification *>::iterator IndicatorDocking::iteratorForChat(const Chat &chat)
{
	IndMMap::iterator end = IndicatorsMap.end();

	if (!chat)
		return end;

	for (IndMMap::iterator it = IndicatorsMap.begin(); it != end; ++it)
		if (it.value()->chat() == chat)
			return it;

	return end;
}

QList<IndicatorDocking::IndMMap::iterator> IndicatorDocking::iteratorsForAggregateChat(const Chat &chat)
{
	QList<IndMMap::iterator> list;

	if (!chat)
		return list;

	Chat aggregateChat = AggregateChatManager::instance()->aggregateChat(chat);
	ChatDetailsAggregate *aggregateChatDetails = qobject_cast<ChatDetailsAggregate *>(aggregateChat.details());
	if (!aggregateChatDetails)
		return list;

	IndMMap::iterator end = IndicatorsMap.end();
	for (IndMMap::iterator it = IndicatorsMap.begin(); it != end; ++it)
		if (aggregateChatDetails->chats().contains(it.value()->chat()))
			list.append(it);

	return list;
}

void IndicatorDocking::createDefaultConfiguration()
{
	config_file.addVariable("Notify", "NewChat_IndicatorNotify", true);
	config_file.addVariable("Notify", "NewMessage_IndicatorNotify", true);
}
