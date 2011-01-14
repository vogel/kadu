/*
 * %kadu copyright begin%
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2002, 2003, 2004, 2006 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2003, 2004 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004, 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008, 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2003, 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2003, 2004 Dariusz Jagodzik (mast3r@kadu.net)
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

#include <algorithm>

#include "emoticons/prefix-node.h"

#include "emots-walker.h"

/** create fresh emoticons dictionary, which will allow easy finding of occurrences
    of stored emots in text
*/
EmotsWalker::EmotsWalker() :
		root(new PrefixNode()), myPair(), positions(), lengths(), amountPositions(0)
{
	myPair.second = NULL;
}

/** deletes entire dictionary of emots */
EmotsWalker::~EmotsWalker()
{
	removeChilds(root);
	delete root;
}

/** find node in prefix tree, which is direct successor of given node with
    edge marked by given character
    return NULL if there is none
*/
PrefixNode* EmotsWalker::findChild(const PrefixNode* node, const QChar &c)
{
	myPair.first = c;
	// create variable 'position' with result of binary search in childs
	// of given node
	QList<Prefix>::const_iterator position = std::upper_bound (node -> children.constBegin(), node -> children.constEnd(), myPair);

	if (position != node -> children.constEnd() && position -> first == c)
		return position -> second;
	else
		return NULL;
}

/** add successor to given node with edge marked by given characted
    (building of prefix tree)
*/
PrefixNode* EmotsWalker::insertChild(PrefixNode* node, const QChar &c)
{
	PrefixNode* newNode = new PrefixNode();

	// create child with new node
	Prefix newPair = qMakePair(c, newNode);
	// insert new child into childs of current node, performing binary
	// search to find correct position for it
	node->children.insert(std::upper_bound(node -> children.begin(), node -> children.end(), newPair), newPair);
	return newNode;
}

/** recursively delete all childs of given node */
void EmotsWalker::removeChilds(PrefixNode *node)
{
	foreach (const Prefix &ch, node->children)
	{
		removeChilds(ch.second);
		delete ch.second;
	}
}

/** adds given string (emot) to dictionary of emots, giving it
    number, which will be used later to notify occurrences of
    emot in analyzed text
*/
void EmotsWalker::insertString(const QString &str, int num)
{
	PrefixNode *child, *node = root;
	unsigned int len = str.length();
	unsigned int pos = 0;

	// it adds string to prefix tree character after character
	while (pos < len) {
		child = findChild(node, str.at(pos));
		if (child == NULL)
			child = insertChild(node, str.at(pos));
		node = child;
		++pos;
	}

	if (node -> emotIndex == -1)
		node -> emotIndex = num;
}

/** return number of emot, which occurre in analyzed text just
    after adding given character (thus ending on this character)
    beginning of text analysis is turned on by 'initWalking()'
    if no emot occurrs, -1 is returned
*/
int EmotsWalker::checkEmotOccurrence(const QChar &c)
{
	const PrefixNode* next;
	int result = -1, resultLen = -1;

	if (amountPositions < positions.size())
	{
		lengths[amountPositions] = 0;
		positions[amountPositions] = root;
		++amountPositions;
	}
	else
	{
		++amountPositions;
		positions.push_back(root);
		lengths.push_back(0);
	}

	for (int i = amountPositions - 1; i >= 0; --i) {
		next = findChild(positions.at(i), c);
		if (next == NULL) {
			--amountPositions;
			lengths[i] = lengths.at(amountPositions);
			positions[i] = positions.at(amountPositions);
		}
		else {
			positions[i] = next;
			++lengths[i];
			if (result == -1 ||
				(next -> emotIndex >= 0 &&
				(next -> emotIndex < result || resultLen < lengths.at(i))))
			{
				resultLen = lengths.at(i);
				result = next -> emotIndex;
			}
		}
	}
	return result;
}

/** clear internal structures responsible for analyzing text, it allows
    begin of new text analysis
*/
void EmotsWalker::initWalking()
{
	amountPositions = 0;
}
