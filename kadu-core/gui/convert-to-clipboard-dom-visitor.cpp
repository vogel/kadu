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

#include "convert-to-clipboard-dom-visitor.h"

ConvertToClipboardDomVisitor::ConvertToClipboardDomVisitor()
{
}

ConvertToClipboardDomVisitor::~ConvertToClipboardDomVisitor()
{
}

QDomNode ConvertToClipboardDomVisitor::visit(QDomText textNode)
{
	return textNode;
}

QDomNode ConvertToClipboardDomVisitor::beginVisit(QDomElement elementNode)
{
	return elementNode;
}

QDomNode ConvertToClipboardDomVisitor::visitImg(QDomElement imgElement)
{
	QString emoticon = imgElement.attribute("emoticon");
	if (emoticon.isEmpty())
		return imgElement;

	QDomText emoticonNode = imgElement.ownerDocument().createTextNode(emoticon);
	imgElement.parentNode().replaceChild(emoticonNode, imgElement);

	return emoticonNode;
}

QDomNode ConvertToClipboardDomVisitor::visitA(QDomElement aElement)
{
	QString display = aElement.attribute("data-display");
	QDomText replacementNode = aElement.ownerDocument().createTextNode(display == aElement.text()
			? aElement.attribute("href")
			: aElement.text());

	aElement.parentNode().replaceChild(replacementNode, aElement);

	return aElement;
}

QDomNode ConvertToClipboardDomVisitor::endVisit(QDomElement elementNode)
{
	if (elementNode.tagName().toLower() == "img")
		return visitImg(elementNode);
	if (elementNode.tagName().toLower() == "a")
		return visitA(elementNode);
	return elementNode;
}
