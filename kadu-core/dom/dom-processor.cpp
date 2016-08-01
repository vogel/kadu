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

#include "dom-processor.h"

#include "dom/dom-visitor.h"

#include <QtXml/QDomNode>

DomProcessor::DomProcessor(QDomDocument &domDocument) :
		m_domDocument{domDocument}
{
}

QDomNode DomProcessor::acceptNode(const DomVisitor *visitor, QDomNode node)
{
	switch (node.nodeType())
	{
		case QDomNode::TextNode:
			node = visitor->visit(node.toText());
			break;
		case QDomNode::ElementNode:
			node = visitor->beginVisit(node.toElement());
			break;
		default:
			break;
	}

	QDomNode childNode = node.firstChild();
	while (!childNode.isNull())
	{
		childNode = acceptNode(visitor, childNode);
		childNode = childNode.nextSibling();
	}

	switch (node.nodeType())
	{
		case QDomNode::ElementNode:
			node = visitor->endVisit(node.toElement());
			break;
		default:
			break;
	}

	return node;
}

void DomProcessor::accept(const DomVisitor *visitor)
{
	Q_ASSERT(visitor);

	acceptNode(visitor, m_domDocument.documentElement());
}
