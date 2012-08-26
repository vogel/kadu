/*
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
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "emoticons/emoticon-prefix-tree.h"
#include "misc/misc.h"

#include "emots-walker.h"

EmotsWalker::EmotsWalker(EmoticonPrefixTree *tree) :
		Tree(tree), positions(), lengths(), amountPositions(0)
{
}

EmotsWalker::~EmotsWalker()
{
}

/** return number of emot, which occurre in analyzed text just
    after adding given character (thus ending on this character)
    beginning of text analysis is turned on by 'initWalking()'
    if no emot occurrs, -1 is returned
*/
Emoticon EmotsWalker::checkEmotOccurrence(QChar c, bool nextIsLetter)
{
	c = extractLetter(c);

	EmoticonPrefixTree *next;
	Emoticon result;
	int resultLen = -1;

	if (amountPositions < positions.size())
	{
		lengths[amountPositions] = 0;
		positions[amountPositions] = Tree.data();
	}
	else
	{
		positions.push_back(Tree.data());
		lengths.push_back(0);
	}

	amountPositions++;

	if (!previousWasLetter || !c.isLetter() || amountPositions > 1)
		for (int i = amountPositions - 1; i >= 0; --i) {
			next = positions.at(i)->child(c);
			if (!next) {
				--amountPositions;
				lengths[i] = lengths.at(amountPositions);
				positions[i] = positions.at(amountPositions);
			}
			else {
				positions[i] = next;
				++lengths[i];
				if (result.isNull() || !next->nodeEmoticon().isNull() || resultLen < lengths.at(i))
				{
					resultLen = lengths.at(i);
					result = next->nodeEmoticon();
				}
			}
		}
	else
		amountPositions = 0;

	previousWasLetter = c.isLetter();

	if (c.isLetter() && nextIsLetter)
		return Emoticon();
	else
		return result;
}

void EmotsWalker::initWalking()
{
	amountPositions = 0;
	previousWasLetter = false;
}
