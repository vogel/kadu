/*
 * %kadu copyright begin%
 * Copyright 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef EMOTICON_PREFIX_TREE_BUILDER_H
#define EMOTICON_PREFIX_TREE_BUILDER_H

#include <QtCore/QScopedPointer>

#include "walker/emoticon-prefix-tree.h"

class QChar;

/**
 * @addtogroup Emoticons
 * @{
 */

/**
 * @class EmoticonPrefixTreeBuilder
 * @short This class takes any number of Emoticon and build an EmoticonPrefixTree from them.
 * @author Rafał 'Vogel' Malinowski
 *
 * Note that tree() method of this class can only be called once.
 */
class EmoticonPrefixTreeBuilder
{
	QScopedPointer<EmoticonPrefixTree> Root;

public:
	EmoticonPrefixTreeBuilder();

	/**
	 * @short Add new emoticon to builded prefix tree.
	 * @param emoticon emoticon to add
	 */
	void addEmoticon(const Emoticon &emoticon);

	/**
	 * @short Return created prefix tree.
	 * @author Rafał 'Vogel' Malinowski
	 * @return created prefix tree.
	 *
	 * Not that this method can only be called once.
	 */
	EmoticonPrefixTree * tree();

};

/**
 * @}
 */

#endif // EMOTICON_PREFIX_TREE_BUILDER_H
