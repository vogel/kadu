/*
 * %kadu copyright begin%
 * Copyright 2016 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "exports.h"

#include <QtWidgets/QWidget>
#include <memory>

class ChatEditBox;
class ChatWidgetTitle;
class Chat;
class CustomInput;
class FormattedString;
class Message;
class NormalizedHtmlString;
class SortedMessages;
class TalkableProxyModel;
class WebkitMessagesView;

enum class ChatState;

class KADUAPI ChatWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChatWidget(QWidget *parent = nullptr);
    virtual ~ChatWidget();

    virtual Chat chat() const = 0;
    virtual ChatState chatState() const = 0;

    virtual void addMessages(const SortedMessages &messages) = 0;
    virtual void addMessage(const Message &message) = 0;
    virtual void appendSystemMessage(NormalizedHtmlString content) = 0;
    virtual SortedMessages messages() const = 0;

    virtual const QDateTime &lastReceivedMessageTime() const = 0;

    virtual void kaduStoreGeometry() = 0;
    virtual void kaduRestoreGeometry() = 0;

    virtual ChatEditBox *getChatEditBox() const = 0;
    virtual ChatWidgetTitle *title() const = 0;
    virtual CustomInput *edit() const = 0;
    virtual TalkableProxyModel *talkableProxyModel() const = 0;
    virtual WebkitMessagesView *chatMessagesView() const = 0;

public slots:
    virtual void sendMessage() = 0;
    virtual void colorSelectorAboutToClose() = 0;
    virtual void clearChatWindow() = 0;

    virtual void requestClose() = 0;

signals:
    void messageReceived(ChatWidget *chatWidget);
    void messageSendRequested(ChatWidget *chat);
    void messageSent(ChatWidget *chat);

    void fileDropped(Chat chat, const QString &fileName);

    void widgetDestroyed(Chat chat);
    void widgetDestroyed(ChatWidget *widget);

    void closeRequested(ChatWidget *chatWidget);
    void chatStateChanged(ChatState state);
};

Q_DECLARE_METATYPE(ChatWidget *);
