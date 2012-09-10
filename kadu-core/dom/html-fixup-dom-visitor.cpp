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

#include <QtXml/QDomElement>
#include <QtXml/QDomText>

#include "html-fixup-dom-visitor.h"

HtmlFixupDomVisitor::HtmlFixupDomVisitor()
{

}

HtmlFixupDomVisitor::~HtmlFixupDomVisitor()
{

}

QDomNode HtmlFixupDomVisitor::visit(QDomText textNode)
{
	return textNode;
}

QDomNode HtmlFixupDomVisitor::fixupImg(QDomElement imgNode)
{
	QDomNode parentNode = imgNode.parentNode();
	int length = imgNode.childNodes().length();
	for (int i = length - 1; i >= 0; i--)
	{
		QDomNode childNode = imgNode.childNodes().at(i);
		parentNode.insertAfter(childNode, imgNode);
	}

	return imgNode;
}

QDomNode HtmlFixupDomVisitor::fixupSpan(QDomElement spanNode)
{
	if (!spanNode.attribute("class").contains("Apple-converted-space"))
		return spanNode;

	QDomNode parentNode = spanNode.parentNode();
	QDomEntityReference spaceNode = parentNode.ownerDocument().createEntityReference("nbsp");
	parentNode.insertAfter(spaceNode, spanNode);
	parentNode.removeChild(spanNode);

	return spaceNode;
}

QDomNode HtmlFixupDomVisitor::beginVisit(QDomElement elementNode)
{
	if (elementNode.tagName().toLower() == "img")
		return fixupImg(elementNode);
	if (elementNode.tagName().toLower() == "span")
		return fixupSpan(elementNode);
	return elementNode;
}

QDomNode HtmlFixupDomVisitor::endVisit(QDomElement elementNode)
{
	return elementNode;
}
