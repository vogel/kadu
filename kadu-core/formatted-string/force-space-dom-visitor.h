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

#include "dom/dom-text-regexp-visitor.h"
#include "exports.h"

#include <QtCore/QObject>

/**
 * @short This DOM Visitor replaces all spaces with &nbsp; and all tabs with &emsp;
 *
 * Used to keep multiple spaces in our HTML documents. QTextDocument::setHtml does not care
 * for that, so we need to.
 */
class KADUAPI ForceSpaceDomVisitor : public DomTextRegexpVisitor
{
public:
    explicit ForceSpaceDomVisitor();
    virtual ~ForceSpaceDomVisitor();

    virtual QList<QDomNode> matchToDomNodes(QDomDocument document, QRegExp regExp) const override;
};
