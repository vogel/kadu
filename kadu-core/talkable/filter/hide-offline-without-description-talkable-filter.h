/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef HIDE_OFFLINE_WITHOUT_DESCRIPTION_TALKABLE_FILTER_H
#define HIDE_OFFLINE_WITHOUT_DESCRIPTION_TALKABLE_FILTER_H

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
 * This filter removes items that not available and have no description. Contact items that have offline
 * status and empty description are rejected. Buddy items that have preffered contact with offline status
 * and no description are rejected. All other items are passed to next filters.
 *
 * This filter can be enable or disabled. In disabled state it always returs Undecided.
 */
class HideOfflineWithoutDescriptionTalkableFilter : public TalkableFilter
{
	Q_OBJECT

	bool Enabled;

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Create new instance of HideOfflineWithoutDescriptionTalkableFilter with given parent.
	 * @param parent QObject parent of new object
	 */
	explicit HideOfflineWithoutDescriptionTalkableFilter(QObject *parent = 0);
	virtual ~HideOfflineWithoutDescriptionTalkableFilter();

	virtual FilterResult filterBuddy(const Buddy &buddy);
	virtual FilterResult filterContact(const Contact &contact);

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

Q_DECLARE_METATYPE(HideOfflineWithoutDescriptionTalkableFilter *)

#endif // HIDE_OFFLINE_WITHOUT_DESCRIPTION_TALKABLE_FILTER_H
