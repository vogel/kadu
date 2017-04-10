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
#include "message/message.h"

#include <QtCore/QObject>

class FormattedString;
class NormalizedHtmlString;

/**
 * @addtogroup Message
 * @{
 */

/**
 * @class MessageManager
 * @short Manager of messages used in Kadu.
 *
 * This singleton class manages messages that go trought Kadu. It also stores all pending messages in permanent storage.
 * Pending message is an incoming message that have not ever been displayed to user.
 */
class KADUAPI MessageManager : public QObject
{
    Q_OBJECT

public:
    explicit MessageManager(QObject *parent = nullptr);
    virtual ~MessageManager();

    /**
     * @short Send new message to given chat.
     * @param chat chat to send message to
     * @param htmlContent HTML content to be sent
     * @param silent if true, no messageSent signal will be emitted
     *
     * This methods sends a message to given chat. Message is passed as HTML string. Protocols are
     * free to ignore any HTML formatting.
     *
     * If silent parameter is true, no messageSent signal will be emitted. This is usefull for plugins
     * like firewall or for sending public keys, as messageSent is usually used to add sent message to
     * chat view.
     */
    virtual bool sendMessage(const Chat &chat, NormalizedHtmlString content, bool silent = false) = 0;

    /**
     * @short Send new raw message to given chat.
     * @param chat chat to send message to
     * @param content raw content to be sent
     *
     * This methods sends a message to given chat. Message is passed as QByteArray.
     *
     * Raw messages will not invoke messageSent signals.
     */
    virtual bool sendRawMessage(const Chat &chat, const QByteArray &content) = 0;

signals:
    /**
     * @short Signal emited every time a message is received from one of registered acocunts.
     * @param message received message
     *
     * This signal is emited every time a message is received from one of registered acocunts. It is not emited
     * if message is added to the system in any other way (e.g. by inserting it manually into chat widget).
     */
    void messageReceived(const Message &message);

    /**
     * @short Signal emited every time a message is sent trought one of registered acocunts.
     * @param message sent message
     *
     * This signal is emited every time a message is sent trought one of registered acocunts.
     */
    void messageSent(const Message &message);
};

/**
 * @}
 */
