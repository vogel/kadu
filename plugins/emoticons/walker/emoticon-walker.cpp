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
 * Copyright 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "emoticon-walker.h"

EmoticonWalker::EmoticonWalker(EmoticonPrefixTree *tree) :
		Tree(tree), PreviousWasLetter(false)
{
}

EmoticonWalker::~EmoticonWalker()
{
}

bool EmoticonWalker::possibleEmoticonStart(QChar c) const
{
	return !(PreviousWasLetter && c.isLetter());
}

bool EmoticonWalker::possibleEmoticonEnd(QChar c, bool nextIsLetter) const
{
	return !(c.isLetter() && nextIsLetter);
}

void EmoticonWalker::addEmptyCandidate()
{
	EmoticonCandidate emptyCandidate;
	emptyCandidate.EmoticonNode = Tree;
	emptyCandidate.EmoticonLength = 0;

	Candidates.append(emptyCandidate);
}

EmoticonPrefixTree * EmoticonWalker::findCandidateExpansion(const EmoticonCandidate &candidate, QChar c)
{
	return candidate.EmoticonNode->child(c);
}

EmoticonWalker::EmoticonCandidate EmoticonWalker::expandCandidate(const EmoticonCandidate &candidate, EmoticonPrefixTree *expansion)
{
	EmoticonCandidate result;
	result.EmoticonNode = expansion;
	result.EmoticonLength = candidate.EmoticonLength + 1;

	return result;
}

void EmoticonWalker::removeCandidate(int i)
{
	if (i != Candidates.count() - 1)
		Candidates.replace(i, Candidates.at(Candidates.count() - 1));
	Candidates.removeLast();
}

void EmoticonWalker::tryExpandAllCandidates(QChar c)
{
	// iterate backward because removeCandidate can switch elements after current one
	for (int i = Candidates.count() - 1; i >= 0; --i)
	{
		EmoticonPrefixTree *expansion = findCandidateExpansion(Candidates.at(i) , c);
		if (expansion)
			Candidates.replace(i, expandCandidate(Candidates.at(i), expansion));
		else
			removeCandidate(i);
	}
}

Emoticon EmoticonWalker::findLongestCandidate() const
{
	Emoticon result;
	int resultLength = -1;

	foreach (const EmoticonCandidate &candidate, Candidates)
		if (result.isNull() || (!candidate.EmoticonNode->nodeEmoticon().isNull() && resultLength < candidate.EmoticonLength))
		{
			result = candidate.EmoticonNode->nodeEmoticon();
			resultLength = candidate.EmoticonLength;
		}

	return result;
}

Emoticon EmoticonWalker::matchEmoticon(QChar c, bool nextIsLetter)
{
	c = extractLetter(c);

	if (Candidates.isEmpty() && !possibleEmoticonStart(c))
		return Emoticon();

	addEmptyCandidate();
	tryExpandAllCandidates(c);

	if (!possibleEmoticonEnd(c, nextIsLetter))
		return Emoticon();

	return findLongestCandidate();
}
