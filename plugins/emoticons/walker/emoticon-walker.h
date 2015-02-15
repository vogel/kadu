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
 * Copyright 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef EMOTICON_WALKER_H
#define EMOTICON_WALKER_H

#include <QtCore/QList>

class QChar;

class Emoticon;
class EmoticonPrefixTree;

/**
 * @addtogroup Emoticons
 * @{
 */

/**
 * @class EmoticonWalker
 * @short Class used to find emoticons in text using EmoticonPrefixTree data structure.
 *
 * For each search new instance must be created. Then for each character a matchEmoticon() must be called.
 * If some matched sequence of characters is equal to trigger text of any emoticon represented by EmoticonPrefixTree
 * then this emoticon is returned.
 */
class EmoticonWalker
{
	EmoticonPrefixTree *Tree;

	struct EmoticonCandidate
	{
		EmoticonPrefixTree *EmoticonNode;
		int EmoticonLength;
	};

	QList<EmoticonCandidate> Candidates;
	bool PreviousWasLetter;

	/**
	 * @short Return true if given character can be a start of new emoticon sequence.
	 * @param c checked character
	 * @return true if given character can be a start of new emoticon sequence
	 *
	 * Emoticons can not start in the middle of word. This methods returns true if last character was not letter
	 * or c is not letter. If boths are letters false is returned.
	 */
	bool possibleEmoticonStart(QChar c) const;

	/**
	 * @short Return true if given character can be a end of new emoticon sequence.
	 * @param c checked character
	 * @return true if given character can be a end of new emoticon sequence
	 *
	 * Emoticons can not end in the middle of word. This methods returns true if next character is not letter
	 * or c is not letter. If boths are letters false is returned.
	 */
	bool possibleEmoticonEnd(QChar c, bool nextIsLetter) const;

	/**
	 * @short Adds empty candidate to list of match candidates
	 */
	void addEmptyCandidate();

	/**
	 * @short Return tree node that expands given candidate by character c.
	 * @param candidate candidate to check
	 * @param c next character to expand
	 * @return tree node that expands given candidate by character c or null
	 */
	EmoticonPrefixTree * findCandidateExpansion(const EmoticonCandidate &candidate, QChar c);

	/**
	 * @short Expand candidate to new tree node.
	 * @param candidate candidate to expand
	 * @param expandion new candidate tree node
	 * @return expanded candidate
	 */
	EmoticonCandidate expandCandidate(const EmoticonCandidate &candidate, EmoticonPrefixTree *expansion);

	/**
	 * @short Remove candidate with given idnex.
	 * @param i index of candidate to remove
	 */
	void removeCandidate(int i);

	/**
	 * @short Try to expand all candidates by a character.
	 * @param c character to expand by
	 *
	 * This method check all candidates for possible expansion by c. If expansion is possible then it is done.
	 * If not, then candidate is removed.
	 */
	void tryExpandAllCandidates(QChar c);

	/**
	 * @short Return emoticon with longest trigger text from matching candidates.
	 * @return emoticon with longest trigger text from matching candidates
	 */
	Emoticon findLongestCandidate() const;

public:
	/**
	 * @short Create new EmoticonWalker instance with given EmoticonPrefixTree.
	 * @param tree EmoticonPrefixTree representation of emoticons to fidn
	 */
	explicit EmoticonWalker(EmoticonPrefixTree *tree);
	~EmoticonWalker();

	/**
	 * @short Try to match emoticon to next character.
	 * @param c next charater
	 * @param nextIsLetter pass true if next character in sequence is letter
	 * @return matched emoticon
	 *
	 * If sequence of characters ending with c is equal to trigger text of any emoticon represented by EmoticonPrefixTree
	 * then this emoticon is returned.
	 */
	Emoticon matchEmoticon(QChar c, bool nextIsLetter);

};

/**
 * @}
 */

#endif // EMOTICON_WALKER_Hs
