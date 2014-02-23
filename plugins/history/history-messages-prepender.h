/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QFuture>
#include <QtCore/QObject>
#include <QtCore/QPointer>

#include "message/message.h"

class SortedMessages;
class WebkitMessagesView;

/**
 * @addtogroup History
 * @{
 */

/**
 * @class HistoryMessagesPrepender
 * @author Rafał 'Vogel' Malinowski
 * @short Class responsible for prepending history messages to chat messages view.
 *
 * This class is used for asynchronous prepending history messages to chat messages view. It takes care of
 * handling destroyed event of used WebkitMessagesView. It also destroys itself after its work is done.
 */
class HistoryMessagesPrepender : public QObject
{
	Q_OBJECT

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Creates new instance of HistoryMessagesPrepender and starts prepending.
	 * @param messages future list of messages to prepend
	 * @param chatMessagesView messages view to display prepended messages
	 *
	 * Calling this constructor is all that is needed with this class. Prependng messages will occur as soon as
	 * messages from QFuture are available. If chatMessagesView is destroyed before that, nothing will happen.
	 * After this class finishes its work (successfully or not) it deletes itself.
	 */
	HistoryMessagesPrepender(QFuture<SortedMessages> messages, WebkitMessagesView *chatMessagesView, QObject *parent = nullptr);
	virtual ~HistoryMessagesPrepender();

private:
	QFuture<SortedMessages> m_messages;
	QPointer<WebkitMessagesView> m_messagesView;

private slots:
	void messagesAvailable();

};

/**
 * @}
 */
