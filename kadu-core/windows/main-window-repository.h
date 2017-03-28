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

#include "exports.h"

#include <QtCore/QObject>
#include <vector>

class QWidget;

class KADUAPI MainWindowRepository : public QObject
{
    Q_OBJECT

public:
    using Storage = std::vector<QWidget *>;
    using Iterator = Storage::iterator;

    Q_INVOKABLE explicit MainWindowRepository(QObject *parent = nullptr);
    virtual ~MainWindowRepository();

    void addMainWindow(QWidget *mainWindow);
    void removeMainWindow(QWidget *mainWindow);

    Iterator begin();
    Iterator end();

signals:
    void mainWindowAdded(QWidget *mainWindow);
    void mainWindowRemoved(QWidget *mainWindow);

private:
    Storage m_data;
};

inline MainWindowRepository::Iterator begin(MainWindowRepository &x)
{
    return x.begin();
}

inline MainWindowRepository::Iterator end(MainWindowRepository &x)
{
    return x.end();
}
