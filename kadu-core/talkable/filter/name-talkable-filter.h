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

#ifndef NAME_TALKABLE_FILTER_H
#define NAME_TALKABLE_FILTER_H

#include "talkable/filter/talkable-filter.h"
#include "exports.h"

/**
 * @addtogroup Talkable
 * @{
 */

/**
 * @class NameTalkableFilter
 * @author Rafał 'Vogel' Malinowski
 * @short Filter that accepts only items matching given name.
 *
 * This filter matches items due to following rules:
 * <ul>
 *   <li>contacts are matched if their id matches given name</li>
 *   <li>buddies are matched if any of theirs contacts is matched, or if one of display, name and email
 *       values matches given name</li>
 *   <li>chats are matched if any of theirs is matched, or if chat display matches given name</li>
 * </ul>
 *
 * This filter may be put in one of two modes:
 * <ul>
 *   <li>AcceptMatching - in this mode item is accepted if matched and rejected if not</li>
 *   <li>UndecidedMatching - in this mode item is passed to next filters if matched and rejected if not</li>
 * </ul>
 */
class KADUAPI NameTalkableFilter : public TalkableFilter
{
	Q_OBJECT

public:
	/**
	 * @enum NameFilterMatchingMode
	 * @author Rafał 'Vogel' Malinowski
	 * @short This enum controls value returned when given item matches name.
	 */
	enum NameFilterMatchingMode
	{
		/**
		 * @short In this mode item is accepted if matched and rejected if not.
		 */
		AcceptMatching,
		/**
		 * @short In this mode item is passed to next filters if matched and rejected if not.
		 */
		UndecidedMatching
	};

private:
	NameFilterMatchingMode Mode;
	QString Name;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns true if given chat matches name.
	 * @param chat chat to match
	 * @return true if given chat matches name
	 */
	bool matches(const Chat &chat);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns true if given buddy matches name.
	 * @param buddy buddy to match
	 * @return true if given buddy matches name
	 */
	bool matches(const Buddy &buddy);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns true if given contact matches name.
	 * @param contact contact to match
	 * @return true if given contact matches name
	 */
	bool matches(const Contact &contact);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Compute FilterResult for given matched value.
	 * @param matched true, if given item matched name, false otherwise
	 * @return FilterResult for given matched value
	 */
	FilterResult computeResult(bool matched);

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Create new instance of NameTalkableFilter with given parent.
	 * @param mode mode controlling return value when given item matches name
	 * @param parent QObject parent of new object
	 */
	explicit NameTalkableFilter(NameFilterMatchingMode mode, QObject *parent = 0);
	virtual ~NameTalkableFilter();

	virtual FilterResult filterChat(const Chat &chat);
	virtual FilterResult filterBuddy(const Buddy &buddy);
	virtual FilterResult filterContact(const Contact &contact);

public slots:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Set new name to match items against.
	 * @param name new name to match items against
	 *
	 * Set new name to match items against. If new name is different than old one then
	 * filterChanged() signal is emited.
	 */
	void setName(const QString &name);

};

/**
 * @}
 */

#endif // NAME_TALKABLE_FILTER_H
