/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat/chat.h"

#include <QtCore/QObject>

class ChatDataWindow;
class InjectedFactory;

class ChatDataWindowRepository : public QObject
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit ChatDataWindowRepository(QObject *parent = nullptr);
    virtual ~ChatDataWindowRepository();

    ChatDataWindow *windowForChat(const Chat &chat);
    const QMap<Chat, ChatDataWindow *> &windows() const;

public slots:
    void showChatWindow(const Chat &chat);

private:
    QPointer<InjectedFactory> m_injectedFactory;

    QMap<Chat, ChatDataWindow *> Windows;

private slots:
    INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);

    void windowDestroyed(const Chat &chat);
};
