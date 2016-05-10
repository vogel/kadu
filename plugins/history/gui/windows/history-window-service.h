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

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class BuddyChatManager;
class Chat;
class Configuration;
class HistoryWindow;
class History;
class HistoryInjectedFactory;

class HistoryWindowService : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit HistoryWindowService(QObject *parent = nullptr);
	virtual ~HistoryWindowService();

	void show(const Chat &chat);

private:
	QPointer<BuddyChatManager> m_buddyChatManager;
	QPointer<Configuration> m_configuration;
	QPointer<HistoryWindow> m_historyWindow;
	QPointer<History> m_history;
	QPointer<HistoryInjectedFactory> m_historyInjectedFactory;

private slots:
	INJEQT_SET void setBuddyChatManager(BuddyChatManager *buddyChatManager);
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_SET void setHistory(History *history);
	INJEQT_SET void setHistoryInjectedFactory(HistoryInjectedFactory *historyInjectedFactory);

};
