/*
 * %kadu copyright begin%
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef TALKABLE_FILTER_H
#define TALKABLE_FILTER_H

#include <QtCore/QObject>

#include "exports.h"

class Buddy;
class Chat;
class Contact;

/**
 * @addtogroup Talkable
 * @{
 */

/**
 * @class TalkableFilter
 * @author Rafał 'Vogel' Malinowski
 * @short Class filtering values of Chat, Buddy or Contact.
 *
 * This class can filter values of Chat, Buddy or Contact. Each filtering action can have
 * one of three results: Accepted, Undecided or Rejected. Accepted means that given objet has been
 * accepted and no more checking needs to be done. Rejected means that given objet has been rejected
 * and no more checking needs to be done. Undecided means that other filter needs to make a decision.
 *
 * Filers can have a configuration. If filter's configuration changes, filterChanged() signal is
 * emited and filtering results needs to be recalculated.
 */
class KADUAPI TalkableFilter : public QObject
{
	Q_OBJECT

public:
	/**
	 * @enum FilterResult
	 * @author Rafał 'Vogel' Malinowski
	 * @short Result of filtering operation.
	 */
	enum FilterResult
	{
		/**
		 * Filtered object is accepted. No more checking is required.
		 */
		Accepted,
		/**
		 * Current filter haven't decided about given obejct. Another filter must do a decision.
		 */
		Undecided,
		/**
		 * Filtered object is rejected. No more checking is required.
		 */
		Rejected
	};

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Create new instance of TalkableFilter with given parent.
	 * @param parent QObject parent of new object
	 */
	explicit TalkableFilter(QObject *parent = 0);
	virtual ~TalkableFilter();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Filter Chat object.
	 * @param chat Chat obejct to filter.
	 *
	 * Filter Chat object. Default implementation returns Undecided.
	 */
	virtual FilterResult filterChat(const Chat &chat);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Filter Buddy object.
	 * @param buddy Buddy obejct to filter.
	 *
	 * Filter Buddy object. Default implementation returns Undecided.
	 */
	virtual FilterResult filterBuddy(const Buddy &buddy);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Filter Contact object.
	 * @param contact Contact obejct to filter.
	 *
	 * Filter Contact object. Default implementation returns Undecided.
	 */
	virtual FilterResult filterContact(const Contact &contact);

signals:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Signal emited when filter configuration has been changed.
	 */
	void filterChanged();

};

/**
 * @}
 */

#endif // TALKABLE_FILTER_H
