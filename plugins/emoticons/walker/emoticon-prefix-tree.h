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
 * Copyright 2008, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
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

#ifndef EMOTICON_PREFIX_TREE_H
#define EMOTICON_PREFIX_TREE_H

#include <QtCore/QMap>

#include "emoticon.h"

/**
 * @addtogroup Emoticons
 * @{
 */

/**
 * @class EmoticonPrefixTree
 * @author Rafał 'Vogel' Malinowski
 * @short Prefix tree build from emoticons trigger texts.
 *
 * Each tree can be a node in a super-tree and can contains sub-tree nodes.
 */
class EmoticonPrefixTree
{
	Emoticon NodeEmoticon;
	QMap<QChar, EmoticonPrefixTree *> Children;

public:
	EmoticonPrefixTree();
	~EmoticonPrefixTree();

	/**
	 * @short Return Emoticon assigned to this node.
	 * @author Rafał 'Vogel' Malinowski
	 * @return Emoticon assigned to this node
	 */
	Emoticon nodeEmoticon() const;

	/**
	 * @short Assign Emoticon to this node.
	 * @author Rafał 'Vogel' Malinowski
	 * @param emoticon new Emoticon to assign to this node
	 */
	void setNodeEmoticon(const Emoticon &emoticon);

	/**
	 * @short Return sub-nodes.
	 * @author Rafał 'Vogel' Malinowski
	 * @return subnodes
	 */
	QMap<QChar, EmoticonPrefixTree *> children() const;

	/**
	 * @short Return sub-node assigned to given next prefix character.
	 * @author Rafał 'Vogel' Malinowski
	 * @param c next prefix character
	 * @return sub-node assigned to given next prefix character
	 *
	 * This method can return null value.
	 */
	EmoticonPrefixTree * child(QChar c);

	/**
	 * @short Return or create and return sub-node assigned to given next prefix character.
	 * @author Rafał 'Vogel' Malinowski
	 * @param c next prefix character
	 * @return sub-node assigned to given next prefix character
	 *
	 * This method can not return null value.
	 */
	EmoticonPrefixTree * createChild(QChar c);

};

/**
 * @}
 */

#endif // EMOTICON_PREFIX_TREE_H
