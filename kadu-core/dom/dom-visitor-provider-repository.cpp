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

#include "dom-visitor-provider-repository.h"

#include "dom/dom-processor.h"
#include "dom/dom-visitor-provider.h"
#include "dom/dom-visitor.h"

#include <QtXml/QDomDocument>

namespace
{
DomVisitorProvider *convert_iterator(DomVisitorProviderRepository::WrappedIterator iterator)
{
    return iterator->second;
}
}

DomVisitorProviderRepository::DomVisitorProviderRepository(QObject *parent) : QObject{parent}
{
}

DomVisitorProviderRepository::~DomVisitorProviderRepository()
{
}

DomVisitorProviderRepository::Iterator DomVisitorProviderRepository::begin() const
{
    return Iterator{m_visitorProviders.begin(), convert_iterator};
}

DomVisitorProviderRepository::Iterator DomVisitorProviderRepository::end() const
{
    return Iterator{m_visitorProviders.end(), convert_iterator};
}

size_t DomVisitorProviderRepository::size() const
{
    return m_visitorProviders.size();
}

void DomVisitorProviderRepository::addVisitorProvider(DomVisitorProvider *visitorProvider, int priority)
{
    auto el = std::make_pair(priority, visitorProvider);
    auto it = std::lower_bound(
        std::begin(m_visitorProviders), std::end(m_visitorProviders), el,
        [](const Item &x, const Item &y) { return x.first < y.first; });

    m_visitorProviders.insert(it, el);
}

void DomVisitorProviderRepository::removeVisitorProvider(DomVisitorProvider *visitorProvider)
{
    auto it = std::find_if(std::begin(m_visitorProviders), std::end(m_visitorProviders), [&](const Item &x) {
        return x.second == visitorProvider;
    });

    if (it != std::end(m_visitorProviders))
        m_visitorProviders.erase(it);
}

#include "dom-visitor-provider-repository.moc"
