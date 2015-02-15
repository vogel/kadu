/*
 * %kadu copyright begin%
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

#include <QtXml/QDomElement>

#include "ignore-links-dom-visitor.h"

IgnoreLinksDomVisitor::IgnoreLinksDomVisitor(DomVisitor *visitor) :
		Visitor(visitor), LinksDepth(0)
{
	Q_ASSERT(Visitor);
}

IgnoreLinksDomVisitor::~IgnoreLinksDomVisitor()
{
}

QDomNode IgnoreLinksDomVisitor::visit(QDomText textNode)
{
	if (0 == LinksDepth)
		return Visitor->visit(textNode);

	return textNode;
}

QDomNode IgnoreLinksDomVisitor::beginVisit(QDomElement elementNode)
{
	if (elementNode.tagName().toLower() == "a")
		LinksDepth++;
	else if (0 == LinksDepth)
		return Visitor->beginVisit(elementNode);

	return elementNode;
}

QDomNode IgnoreLinksDomVisitor::endVisit(QDomElement elementNode)
{
	if (elementNode.tagName().toLower() == "a")
		LinksDepth--;
	else if (0 == LinksDepth)
		return Visitor->endVisit(elementNode);

	return elementNode;
}
