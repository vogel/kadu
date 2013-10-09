/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef MOBILE_TALKABLE_FILTER_H
#define MOBILE_TALKABLE_FILTER_H

#include "talkable/filter/talkable-filter.h"
#include "exports.h"

/**
 * @addtogroup Talkable
 * @{
 */

/**
 * @class MobileTalkableFilter
 * @author Rafał 'Vogel' Malinowski
 * @short Filter that rejects buddies without mobile number.
 *
 * This filter rejects contacts, chats and buddies without mobile number. Buddies
 * with mobile number are passed to next filters.
 */
class KADUAPI MobileTalkableFilter : public TalkableFilter
{
	Q_OBJECT

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Create new instance of MobileTalkableFilter with given parent.
	 * @param parent QObject parent of new object
	 */
	explicit MobileTalkableFilter(QObject *parent = 0);
	virtual ~MobileTalkableFilter();

	virtual FilterResult filterChat(const Chat &chat);
	virtual FilterResult filterBuddy(const Buddy &buddy);
	virtual FilterResult filterContact(const Contact &contact);

};

/**
 * @}
 */

#endif // MOBILE_TALKABLE_FILTER_H
