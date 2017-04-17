/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat-notifier.h"
#include "chat-notifier.moc"

#include "contacts/contact-set.h"
#include "formatted-string/formatted-string-factory.h"
#include "html/html-conversion.h"
#include "html/html-string.h"
#include "message/message-manager.h"
#include "notification/notification.h"
#include "widgets/chat-widget/chat-widget-repository.h"
#include "widgets/chat-widget/chat-widget.h"

ChatNotifier::ChatNotifier(QObject *parent)
        : QObject{parent}, Notifier{"ChatNotifier", QT_TRANSLATE_NOOP("@default", "Chat window notifications"),
                                    KaduIcon("protocols/common/message")}
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

NotifierConfigurationWidget *ChatNotifier::createConfigurationWidget(QWidget *parent)
{
    Q_UNUSED(parent);
    return 0;
}

void ChatNotifier::sendNotificationToChatWidget(const Notification &notification, ChatWidget *chatWidget)
{
    if (!m_formattedStringFactory)
        return;

    auto content =
        notification.details.string().isEmpty()
            ? notification.text
            : normalizeHtml(HtmlString{"%1<br/> <small>%2</small>"}).arg(notification.text, notification.details);

    chatWidget->appendSystemMessage(content);
}

void ChatNotifier::notify(const Notification &notification)
{
    if (!m_chatWidgetRepository)
        return;

    auto buddies = BuddySet();
    auto chat = notification.data["chat"].value<Chat>();
    if (chat)
        buddies = chat.contacts().toBuddySet();

    for (auto chatWidget : m_chatWidgetRepository.data())
        // warning: do not exchange intersect caller and argument, it will modify buddies variable if you do
        if (buddies.isEmpty() || !chatWidget->chat().contacts().toBuddySet().intersect(buddies).isEmpty())
            sendNotificationToChatWidget(notification, chatWidget);
}
