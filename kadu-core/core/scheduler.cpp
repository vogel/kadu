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

#include "scheduler.h"

#include "chat/recent-chat-service.h"

Scheduler::Scheduler(QObject *parent) :
		QObject{parent}
{
}

Scheduler::~Scheduler()
{
}

void Scheduler::setRecentChatService(RecentChatService *recentChatService)
{
	m_recentChatService = recentChatService;
}

void Scheduler::init()
{
	connect(&m_everyMinuteAction, &QTimer::timeout, this, &Scheduler::everyMinuteAction);
	m_everyMinuteAction.setInterval(60 * 1000);
	m_everyMinuteAction.start();
}

void Scheduler::everyMinuteAction()
{
	m_recentChatService->cleanUp();
}
