/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtXml/QDomText>

#include "walker/emoticon-walker.h"
#include "emoticon.h"

#include "emoticon-expander.h"

EmoticonExpander::EmoticonExpander(EmoticonPrefixTree *tree, EmoticonPathProvider *pathProvider) :
		Tree(tree), PathProvider(pathProvider)
{
	Q_ASSERT(Tree);
	Q_ASSERT(PathProvider);
}

EmoticonExpander::~EmoticonExpander()
{
}

QDomText EmoticonExpander::insertEmoticon(QDomText textNode, const Emoticon &emoticon, int index)
{
	int emoticonLength = emoticon.triggerText().length();

	QDomText afterEmoticon = textNode.splitText(index + emoticonLength);
	textNode.setNodeValue(textNode.nodeValue().mid(0, index));

	QDomElement emoticonElement = textNode.ownerDocument().createElement("img");
	emoticonElement.setAttribute("emoticon", emoticon.triggerText());
	emoticonElement.setAttribute("title", emoticon.triggerText());
	emoticonElement.setAttribute("alt", emoticon.triggerText());
	emoticonElement.setAttribute("src", "file:///" + PathProvider->emoticonPath(emoticon));
	textNode.parentNode().insertBefore(emoticonElement, afterEmoticon);

	return afterEmoticon;
}

QDomText EmoticonExpander::expandFirstEmoticon(QDomText textNode)
{
	QString text = textNode.nodeValue().toLower();
	int textLength = text.length();

	if (0 == textLength)
		return QDomText();

	int currentEmoticonStart = -1;
	Emoticon currentEmoticon;

	EmoticonWalker walker(Tree);

	for (int i = 0; i < textLength; i++)
	{
		Emoticon emoticon = walker.matchEmoticon(text.at(i), (i < textLength - 1) && text.at(i + 1).isLetter());
		if (emoticon.isNull())
			continue;

		// TODO: remove this dependency
		int emoticonStart = i - emoticon.triggerText().length() + 1;
		if (currentEmoticon.isNull() || currentEmoticonStart >= emoticonStart)
		{
			currentEmoticon = emoticon;
			currentEmoticonStart = emoticonStart;
			continue;
		}

		return insertEmoticon(textNode, currentEmoticon, currentEmoticonStart);
	}

	if (!currentEmoticon.isNull())
		insertEmoticon(textNode, currentEmoticon, currentEmoticonStart);

	return QDomText();
}

QDomNode EmoticonExpander::visit(QDomText textNode)
{
	QDomText result = textNode;
	while (!textNode.isNull())
	{
		result = textNode;
		textNode = expandFirstEmoticon(textNode);
	}

	return result; // last real node
}

QDomNode EmoticonExpander::beginVisit(QDomElement elementNode)
{
	return elementNode;
}

QDomNode EmoticonExpander::endVisit(QDomElement elementNode)
{
	return elementNode;
}
