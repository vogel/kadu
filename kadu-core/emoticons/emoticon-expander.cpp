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

#include <QtXml/QDomText>

#include "emoticons/emoticons-manager.h"
#include "emoticons/emoticon-path-provider.h"
#include "emoticons/emots-walker.h"

#include "emoticon-expander.h"

EmoticonExpander::EmoticonExpander(EmotsWalker* emoticonWalker, EmoticonPathProvider *pathProvider) :
		EmoticonWalker(emoticonWalker), PathProvider(pathProvider)
{
	Q_ASSERT(EmoticonWalker);
	Q_ASSERT(PathProvider);
}

EmoticonExpander::~EmoticonExpander()
{
}

QDomText EmoticonExpander::insertEmoticon(QDomText textNode, const EmoticonsManager::EmoticonsListItem &emoticon, int index)
{
	int emoticonLength = emoticon.alias.length();

	QDomText afterEmoticon = textNode.splitText(index + emoticonLength);
	textNode.setNodeValue(textNode.nodeValue().mid(0, index));

	QDomElement emoticonElement = textNode.ownerDocument().createElement("img");
	emoticonElement.setAttribute("emoticon", emoticon.alias);
	emoticonElement.setAttribute("title", emoticon.alias);
	emoticonElement.setAttribute("alt", emoticon.alias);
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
	int currentEmoticonIndex = -1;

	EmoticonWalker->initWalking();
	for (int i = 0; i < textLength; i++)
	{
		int emoticonIndex = EmoticonWalker->checkEmotOccurrence(text.at(i), (i < textLength - 1) && text.at(i + 1).isLetter());
		if (emoticonIndex < 0)
			continue;

		// TODO: remove this dependency
		int emoticonStart = i - EmoticonsManager::instance()->aliases().at(emoticonIndex).alias.length() + 1;
		if (currentEmoticonIndex < 0 || currentEmoticonStart >= emoticonStart)
		{
			currentEmoticonIndex = emoticonIndex;
			currentEmoticonStart = emoticonStart;
			continue;
		}

		const EmoticonsManager::EmoticonsListItem &emoticon = EmoticonsManager::instance()->aliases().at(currentEmoticonIndex);
		return insertEmoticon(textNode, emoticon, currentEmoticonStart);
	}

	if (currentEmoticonIndex >= 0)
	{
		// TODO: remove this dependency
		const EmoticonsManager::EmoticonsListItem &emoticon = EmoticonsManager::instance()->aliases().at(currentEmoticonIndex);
		insertEmoticon(textNode, emoticon, currentEmoticonStart);
	}

	return QDomText();
}

void EmoticonExpander::visit(QDomText textNode)
{
	while (!textNode.isNull())
		textNode = expandFirstEmoticon(textNode);
}
