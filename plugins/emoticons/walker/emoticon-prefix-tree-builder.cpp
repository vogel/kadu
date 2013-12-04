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

#include "misc/misc.h"

#include "walker/emoticon-prefix-tree.h"

#include "emoticon-prefix-tree-builder.h"

EmoticonPrefixTreeBuilder::EmoticonPrefixTreeBuilder() :
		Root(new EmoticonPrefixTree())
{
}

void EmoticonPrefixTreeBuilder::addEmoticon(const Emoticon &emoticon)
{
	Q_ASSERT(Root);

	QString text = emoticon.triggerText().toLower();
	int length = text.length();

	EmoticonPrefixTree *node = Root.data();
	for (int i = 0; i < length; i++)
	{
		QChar c = extractLetter(text.at(i));

		EmoticonPrefixTree *child = node->child(c);
		if (!child)
			child = node->createChild(c);
		node = child;
	}

	if (node->nodeEmoticon().isNull())
		node->setNodeEmoticon(emoticon);
}

EmoticonPrefixTree * EmoticonPrefixTreeBuilder::tree()
{
	return Root.take();
}
