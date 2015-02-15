/*
 * %kadu copyright begin%
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

#ifndef HIDE_WITHOUT_DESCRIPTION_TALKABLE_FILTER_H
#define HIDE_WITHOUT_DESCRIPTION_TALKABLE_FILTER_H

#include <QtCore/QMetaType>

#include "talkable/filter/talkable-filter.h"

/**
 * @addtogroup Talkable
 * @{
 */

/**
 * @class HideOfflineTalkableFilter
 * @author Rafał 'Vogel' Malinowski
 * @short Filter that removes items that are not available and have no description.
 *
 * This filter removes items that have no description. Contact items that have empty description are rejected.
 * Buddy items that have preffered contact with no description are rejected. All other items are passed
 * to next filters.
 *
 * This filter can be enable or disabled. In disabled state it always returs Undecided.
 */
class HideWithoutDescriptionTalkableFilter : public TalkableFilter
{
	Q_OBJECT

	bool Enabled;

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Create new instance of HideWithoutDescriptionTalkableFilter with given parent.
	 * @param parent QObject parent of new object
	 */
	explicit HideWithoutDescriptionTalkableFilter(QObject *parent = 0);
	virtual ~HideWithoutDescriptionTalkableFilter();

	virtual FilterResult filterBuddy(const Buddy &buddy);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Enable or disalbe filter.
	 * @param enabled new value of enabled property
	 */
	void setEnabled(bool enabled);

};

/**
 * @}
 */

Q_DECLARE_METATYPE(HideWithoutDescriptionTalkableFilter *)

#endif // HIDE_WITHOUT_DESCRIPTION_TALKABLE_FILTER_H
