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

#include "emoticon-walker.h"

EmoticonWalker::EmoticonWalker(EmoticonPrefixTree *tree) :
		Tree(tree), PreviousWasLetter(false)
{
}

EmoticonWalker::~EmoticonWalker()
{
}

Emoticon EmoticonWalker::checkEmotOccurrence(QChar c, bool nextIsLetter)
{
	c = extractLetter(c);

	EmoticonPrefixTree *next;
	Emoticon result;
	int resultLen = -1;

	EmoticonCandidate emptyCandidate;
	emptyCandidate.EmoticonNode = Tree;

	Candidates.append(emptyCandidate);

	if (!PreviousWasLetter || !c.isLetter() || Candidates.count() > 1)
		for (int i = Candidates.count() - 1; i >= 0; --i) {
			next = Candidates.at(i).EmoticonNode->child(c);
			if (!next) {
				Candidates.replace(i, Candidates.at(Candidates.count() - 1));
				Candidates.removeLast();
			}
			else {
				Candidates[i].EmoticonNode = next;
				Candidates[i].EmoticonLength++;
				if (result.isNull() || !next->nodeEmoticon().isNull() || resultLen < Candidates[i].EmoticonLength)
				{
					resultLen = Candidates[i].EmoticonLength;
					result = next->nodeEmoticon();
				}
			}
		}
	else
		Candidates.clear();

	PreviousWasLetter = c.isLetter();

	if (c.isLetter() && nextIsLetter)
		return Emoticon();
	else
		return result;
}
