/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QFutureWatcher>

#include "gui/widgets/chat-messages-view.h"

#include "history-messages-prepender.h"

HistoryMessagesPrepender::HistoryMessagesPrepender(QFuture<QVector<Message> > messages, ChatMessagesView *chatMessagesView) :
		Messages(messages), MessagesView(chatMessagesView)
{
	Q_ASSERT(MessagesView);

	connect(MessagesView.data(), SIGNAL(destroyed()), this, SLOT(deleteLater()));

	QFutureWatcher<QVector<Message> > *futureWatcher = new QFutureWatcher<QVector<Message> >(this);
	connect(futureWatcher, SIGNAL(finished()), this, SLOT(messagesAvailable()));

	futureWatcher->setFuture(Messages);
}

HistoryMessagesPrepender::~HistoryMessagesPrepender()
{
}

void HistoryMessagesPrepender::messagesAvailable()
{
	if (!MessagesView)
		return;

	MessagesView.data()->prependMessages(Messages.result());
	MessagesView.clear();
	deleteLater();
}
