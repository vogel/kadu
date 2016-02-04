/*
 * %kadu copyright begin%
 * Copyright 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QCoreApplication>

#include "talkable/talkable.h"

#include "history.h"

#include "buddy-history-delete-handler.h"

BuddyHistoryDeleteHandler::BuddyHistoryDeleteHandler(QObject *parent) :
		QObject{parent}
{
}

BuddyHistoryDeleteHandler::~BuddyHistoryDeleteHandler()
{
}

void BuddyHistoryDeleteHandler::setHistory(History *history)
{
	m_history = history;
}

QString BuddyHistoryDeleteHandler::name()
{
	return QStringLiteral("history-delete-handler");
}

QString BuddyHistoryDeleteHandler::displayName()
{
	return QCoreApplication::translate("BuddyHistoryDeleteHandler", "Chat history");
}

void BuddyHistoryDeleteHandler::deleteBuddyAdditionalData(Buddy buddy)
{
	if (m_history->currentStorage())
		m_history->currentStorage()->deleteHistory(buddy);
}

#include "moc_buddy-history-delete-handler.cpp"
