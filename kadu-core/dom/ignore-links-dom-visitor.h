/*
 * %kadu copyright begin%
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "dom/dom-visitor.h"
#include "exports.h"

#include <QtCore/QScopedPointer>
#include <memory>

/**
 * @addtogroup Dom
 * @{
 */

/**
 * @class IgnoreLinksDomVisitor
 * @short Proxy visitor that hides all content inside A elements.
 *
 * This visitor acts like proxy on top of other visitor. It hides content of A elements before proxied visitor.
 * It also takes care of deleting proxied visitor.
 */
class KADUAPI IgnoreLinksDomVisitor : public DomVisitor
{
public:
    /**
     * @short Create new IgnoreLinksDomVisitor over visitor.
     * @param visitor this visitor will get all data about processed DOM document with exception of content of A
     * elements
     *
     * IgnoreLinksDomVisitor takes care of deleting visitor instance when not needed anymore. Do not delete it manually.
     */
    explicit IgnoreLinksDomVisitor(std::unique_ptr<DomVisitor> visitor);
    virtual ~IgnoreLinksDomVisitor();

    DomVisitor *visitor() const
    {
        return m_visitor.get();
    }

    virtual QDomNode visit(QDomText textNode) const;
    virtual QDomNode beginVisit(QDomElement elementNode) const;
    virtual QDomNode endVisit(QDomElement elementNode) const;

private:
    std::unique_ptr<DomVisitor> m_visitor;
    mutable int m_linksDepth;
};

/**
 * @}
 */
