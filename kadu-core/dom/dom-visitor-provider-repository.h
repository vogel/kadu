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
#include "misc/iterator.h"

#include <QtCore/QMap>
#include <QtCore/QObject>

class DomVisitorProvider;

class KADUAPI DomVisitorProviderRepository : public QObject
{
    Q_OBJECT

    using Item = std::pair<int, DomVisitorProvider *>;
    using Storage = std::vector<Item>;

public:
    using WrappedIterator = Storage::const_iterator;
    using Iterator = IteratorWrapper<WrappedIterator, DomVisitorProvider *>;

    Q_INVOKABLE explicit DomVisitorProviderRepository(QObject *parent = nullptr);
    virtual ~DomVisitorProviderRepository();

    /**
     * @short Register new DomVisitorProvider with given priority.
     * @author Rafał 'Vogel' Malinowski
     * @param visitorProvider visitor provider to register
     * @param priority priority of new provider, smaller priorities are called first
     *
     * If given DomVisitorProvider is already registered, nothing will happen.
     */
    void addVisitorProvider(DomVisitorProvider *visitorProvider, int priority);

    /**
     * @short Unegister new DomVisitorProvider with given priority.
     * @author Rafał 'Vogel' Malinowski
     * @param visitorProvider visitor provider to unregister
     *
     * If given DomVisitorProvider is not registered, nothing will happen.
     */
    void removeVisitorProvider(DomVisitorProvider *visitorProvider);

    Iterator begin() const;
    Iterator end() const;
    std::size_t size() const;

private:
    Storage m_visitorProviders;

    void updateVisitors() const;
};

inline DomVisitorProviderRepository::Iterator begin(DomVisitorProviderRepository *domVisitorProviderRepository)
{
    return domVisitorProviderRepository->begin();
}

inline DomVisitorProviderRepository::Iterator end(DomVisitorProviderRepository *domVisitorProviderRepository)
{
    return domVisitorProviderRepository->end();
}
