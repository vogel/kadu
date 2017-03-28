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

class Notifier;

class KADUAPI NotifierRepository : public QObject
{
    Q_OBJECT

    using Storage = std::vector<Notifier *>;

public:
    using Iterator = Storage::iterator;

    Q_INVOKABLE explicit NotifierRepository(QObject *parent = nullptr);
    virtual ~NotifierRepository();

    void registerNotifier(Notifier *notifier);
    void unregisterNotifier(Notifier *notifier);

    std::size_t size() const
    {
        return m_notifiers.size();
    }

    Iterator begin()
    {
        return std::begin(m_notifiers);
    }
    Iterator end()
    {
        return std::end(m_notifiers);
    }

signals:
    void notifierRegistered(Notifier *notifier);
    void notifierUnregistered(Notifier *notifier);

private:
    Storage m_notifiers;
};

inline NotifierRepository::Iterator begin(NotifierRepository *notifierRepository)
{
    return notifierRepository->begin();
}

inline NotifierRepository::Iterator end(NotifierRepository *notifierRepository)
{
    return notifierRepository->end();
}
