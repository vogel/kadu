/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QStack>
#include <QtXml/QDomNode>

#include "dom/dom-visitor.h"

#include "dom-processor.h"

DomProcessor::DomProcessor(QDomDocument &domDocument) :
		DomDocument(domDocument)
{
}

QDomNode DomProcessor::acceptNode(DomVisitor *visitor, QDomNode node)
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

void DomProcessor::accept(DomVisitor *visitor)
{
	Q_ASSERT(visitor);

	acceptNode(visitor, DomDocument.documentElement());
}
