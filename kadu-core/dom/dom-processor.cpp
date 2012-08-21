/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

void DomProcessor::accept(DomVisitor *visitor)
{
	Q_ASSERT(visitor);

	QStack<QDomNode> nodes;
	nodes.push(DomDocument.documentElement());

	while (!nodes.isEmpty())
	{
		QDomNode node = nodes.pop();

		QDomNodeList childNodes = node.childNodes();
		uint childNodesLength = childNodes.length();
		for (uint i = 0; i < childNodesLength; i++)
			nodes.append(childNodes.at(i));

		switch (node.nodeType())
		{
			case QDomNode::TextNode:
				visitor->visit(node.toText());
				break;
			default:
				break;
		}
	}
}
