/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010, 2011, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "serial-delayed-task-executor.h"
#include "serial-delayed-task-executor.moc"

#include <QtCore/QTimer>

namespace
{
auto const DELAY = 500;
};

SerialDelayedTaskExecutor::SerialDelayedTaskExecutor(QObject *parent) : QObject{parent}
{
    m_timer.setInterval(DELAY);
    connect(&m_timer, &QTimer::timeout, this, &SerialDelayedTaskExecutor::executeNext);
}

SerialDelayedTaskExecutor::~SerialDelayedTaskExecutor()
{
}

void SerialDelayedTaskExecutor::execute(Task task)
{
    if (!task)
        return;

    m_tasks.push(std::move(task));
    if (!m_timer.isActive())
        m_timer.start();
}

void SerialDelayedTaskExecutor::executeNext()
{
    while (!m_tasks.empty())
    {
        auto task = m_tasks.front();
        m_tasks.pop();

        if (task())
            break;
    }

    if (m_tasks.empty())
        m_timer.stop();
}
