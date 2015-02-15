/*
 * %kadu copyright begin%
 * Copyright 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "history-messages-prepender.h"

#include "gui/widgets/webkit-messages-view/webkit-messages-view.h"
#include "message/sorted-messages.h"

#include <QtCore/QFutureWatcher>

HistoryMessagesPrepender::HistoryMessagesPrepender(QFuture<SortedMessages> messages, WebkitMessagesView *chatMessagesView, QObject *parent) :
		QObject{parent},
		m_messages{std::move(messages)},
		m_messagesView(chatMessagesView)
{
	Q_ASSERT(m_messagesView);

	connect(m_messagesView, SIGNAL(destroyed()), this, SLOT(deleteLater()));

	auto futureWatcher = make_owned<QFutureWatcher<SortedMessages>>(this);
	connect(futureWatcher.get(), SIGNAL(finished()), this, SLOT(messagesAvailable()));

	futureWatcher->setFuture(m_messages);
}

HistoryMessagesPrepender::~HistoryMessagesPrepender()
{
}

void HistoryMessagesPrepender::messagesAvailable()
{
	if (!m_messagesView)
		return;

	m_messagesView->setForcePruneDisabled(true);
	m_messagesView->add(m_messages.result());
	m_messagesView = nullptr;
	deleteLater();
}

#include "moc_history-messages-prepender.cpp"
