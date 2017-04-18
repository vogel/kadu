/*
 * %kadu copyright begin%
 * Copyright 2017 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "task/task.h"

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <injeqt/injeqt.h>
#include <queue>

class AggregatedContactAvatarService;
struct ContactAvatarGlobalId;

class KADUAPI SerialDelayedTaskExecutor : public QObject
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit SerialDelayedTaskExecutor(QObject *parent = nullptr);
    virtual ~SerialDelayedTaskExecutor();

    void execute(Task task);

private:
    std::queue<Task> m_tasks;
    QTimer m_timer;

private slots:
    void executeNext();
};
