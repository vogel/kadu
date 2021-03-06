/*
 * %kadu copyright begin%
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

#include "status/status-changer.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class AutostatusStatusChanger : public StatusChanger
{
    Q_OBJECT

    bool Enabled;

    // TODO: we have enums in C++
    // 0 - online
    // 1 - budy
    // 2 - invisible
    int StatusIndex;
    QString Description;

public:
    Q_INVOKABLE explicit AutostatusStatusChanger(QObject *parent = nullptr);
    virtual ~AutostatusStatusChanger();

    void setEnabled(bool enabled);
    void setConfiguration(int statusIndex, const QString &description);

    virtual void changeStatus(StatusContainer *container, Status &status);
};
