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

#ifndef EMOTS_WALKER_H
#define EMOTS_WALKER_H

#include <QtCore/QPair>

struct PrefixNode;

/**
	this class serves as dictionary of emots, allowing easy
	finding of their occurrences in text;
	new search is initialized by calling 'initWalking()'
	then characters are put into analysis by 'checkEmotOccurrence(c)'
*/
class EmotsWalker
{
	/** dictionary is based on prefix tree */
	PrefixNode *root;
	QPair<QChar, PrefixNode *> myPair;
	/** positions in prefix tree, representing current analysis of text */
	QList<const PrefixNode *> positions;
	QList<int> lengths;
	int amountPositions;

	PrefixNode * findChild(const PrefixNode *node, const QChar &c);
	PrefixNode * insertChild(PrefixNode *node, const QChar &c);
	void removeChilds(PrefixNode *node);

public:
	EmotsWalker();
	~EmotsWalker();

	/**
		adds given string (emot) to dictionary of emots, giving it
		number, which will be used later to notify occurrences of
		emot in analyzed text
	*/
	void insertString(const QString &str, int num);

	/**
		return number of emot, which occurre in analyzed text just
		after adding given character (thus ending on this character)
		beginning of text analysis is turned on by 'initWalking()'
		if no emot occures, -1 is returned
	*/
	int checkEmotOccurrence(const QChar &c);

	/**
		clear internal structures responsible for analyzing text, it allows
		begin of new text analysis
	*/
	void initWalking();

};

#endif // EMOTS_WALKER_H
