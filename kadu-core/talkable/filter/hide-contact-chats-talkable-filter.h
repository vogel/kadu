/*
 * %kadu copyright begin%
 * Copyright 2004 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2002, 2003, 2004, 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2002, 2003, 2004 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2009, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2005 Marcin Ślusarz (joi@kadu.net)
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

#ifndef HIDE_CONTACT_CHATS_TALKABLE_FILTER_H
#define HIDE_CONTACT_CHATS_TALKABLE_FILTER_H

#include "talkable/filter/talkable-filter.h"

/**
 * @addtogroup Talkable
 * @{
 */

/**
 * @class HideContactChatsTalkableFilter
 * @author Rafał 'Vogel' Malinowski
 * @short Filter that removes items that are chats with only one contact.
 *
 * This filter removes items that are chats with only one contact. This allows removing double items
 * from roster widget, as single contacts are non-distinguishable from single contact chats.
 */
class HideContactChatsTalkableFilter : public TalkableFilter
{
	Q_OBJECT

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Create new instance of HideContactChatsTalkableFilter with given parent.
	 * @param parent QObject parent of new object
	 */
	explicit HideContactChatsTalkableFilter(QObject *parent = 0);
	virtual ~HideContactChatsTalkableFilter();

	virtual FilterResult filterChat(const Chat &chat);

};

/**
 * @}
 */

#endif // HIDE_SIMPLE_CHATS_TALKABLE_FILTER_H
