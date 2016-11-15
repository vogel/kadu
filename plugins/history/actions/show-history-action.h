/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QPointer>
#include <QtWidgets/QAction>
#include <injeqt/injeqt.h>

#include "history-exports.h"

#include "gui/actions/action-description.h"

class BuddyChatManager;
class HistoryWindowService;
class History;

class HISTORYAPI ShowHistoryAction : public ActionDescription
{
	Q_OBJECT

	QPointer<BuddyChatManager> m_buddyChatManager;
	QPointer<HistoryWindowService> m_historyWindowService;
	QPointer<History> m_history;

	int ChatHistoryQuotationTime;

	void showDaysMessages(QAction *action, int days);

private slots:
	INJEQT_SET void setBuddyChatManager(BuddyChatManager *buddyChatManager);
	INJEQT_SET void setHistoryWindowService(HistoryWindowService *historyWindowService);
	INJEQT_SET void setHistory(History *history);
	INJEQT_INIT void init();

	void showPruneMessages();
	void showOneDayMessages();
	void show7DaysMessages();
	void show30DaysMessages();
	void showAllMessages();

protected:
	virtual void configurationUpdated();

	virtual void actionInstanceCreated(Action *action);
	virtual void actionTriggered(QAction *sender, bool toggled);

public:
	Q_INVOKABLE explicit ShowHistoryAction(QObject *parent = nullptr);
	virtual ~ShowHistoryAction();

};
