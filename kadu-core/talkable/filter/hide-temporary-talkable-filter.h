/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef HIDE_TEMPORARY_TALKABLE_FILTER_H
#define HIDE_TEMPORARY_TALKABLE_FILTER_H

#include <QtCore/QMetaType>

#include "exports.h"
#include "talkable/filter/talkable-filter.h"

/**
 * @addtogroup Talkable
 * @{
 */

/**
 * @class HideTemporaryTalkableFilter
 * @author Rafał 'Vogel' Malinowski
 * @short Filter that removes temporary items.
 *
 * This filter removes temporary items. An example of temporary item is Contat or Buddy from XMPP MUC chat.
 */
class KADUAPI HideTemporaryTalkableFilter : public TalkableFilter
{
	Q_OBJECT

	bool Enabled;

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Create new instance of HideTemporaryTalkableFilter with given parent.
	 * @param parent QObject parent of new object
	 */
	explicit HideTemporaryTalkableFilter(QObject *parent = 0);
	virtual ~HideTemporaryTalkableFilter();

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

Q_DECLARE_METATYPE(HideTemporaryTalkableFilter *)

#endif // HIDE_TEMPORARY_TALKABLE_FILTER_H
