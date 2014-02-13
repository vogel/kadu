/*
 * %kadu copyright begin%
 * Copyright 2009, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#include "contacts/contact-set.h"
#include "formatted-string/formatted-string-factory.h"
#include "gui/widgets/chat-widget/chat-widget-repository.h"
#include "gui/widgets/chat-widget/chat-widget.h"
#include "message/message-manager.h"
#include "notify/notification/aggregate-notification.h"
#include "notify/notification/chat-notification.h"

#include "chat-notifier.h"

ChatNotifier::ChatNotifier(QObject *parent) :
		Notifier("ChatNotifier", QT_TRANSLATE_NOOP("@default", "Chat window notifications"), KaduIcon("protocols/common/message"), parent)
{
}

ChatNotifier::~ChatNotifier()
{
}

void ChatNotifier::setChatWidgetRepository(ChatWidgetRepository *chatWidgetRepository)
{
	m_chatWidgetRepository = chatWidgetRepository;
}

void ChatNotifier::setFormattedStringFactory(FormattedStringFactory *formattedStringFactory)
{
	m_formattedStringFactory = formattedStringFactory;
}

NotifierConfigurationWidget * ChatNotifier::createConfigurationWidget(QWidget* parent)
{
	Q_UNUSED(parent);
	return 0;
}

void ChatNotifier::sendNotificationToChatWidget(Notification *notification, ChatWidget *chatWidget)
{
	if (!m_formattedStringFactory)
		return;

	auto content = notification->text();
	if (!notification->details().isEmpty())
		content += "<br/> <small>" + notification->details().join("<br/>") + "</small>";

	chatWidget->appendSystemMessage(m_formattedStringFactory.data()->fromHtml(content));
}

void ChatNotifier::notify(Notification *notification)
{
	if (!m_chatWidgetRepository)
		return;

	auto aggregateNotification = qobject_cast<AggregateNotification *>(notification);
	if (!aggregateNotification)
		return;

	auto latestNotification = aggregateNotification->notifications().last();

	auto buddies = BuddySet();
	auto chatNotification = qobject_cast<ChatNotification *>(latestNotification);
	if (chatNotification)
		buddies = chatNotification->chat().contacts().toBuddySet();

	for (auto chatWidget : m_chatWidgetRepository.data())
		// warning: do not exchange intersect caller and argument, it will modify buddies variable if you do
		if (buddies.isEmpty() || !chatWidget->chat().contacts().toBuddySet().intersect(buddies).isEmpty())
			sendNotificationToChatWidget(latestNotification, chatWidget);
}

#include "moc_chat-notifier.cpp"
