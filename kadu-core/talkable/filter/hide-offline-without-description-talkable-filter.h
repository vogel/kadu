/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

class HideOfflineWithoutDescriptionTalkableFilter : public TalkableFilter
{
	Q_OBJECT

	bool Enabled;

public:
	explicit HideOfflineWithoutDescriptionTalkableFilter(QObject *parent = 0);
	virtual ~HideOfflineWithoutDescriptionTalkableFilter();

	virtual FilterResult filterBuddy(const Buddy &buddy);
	virtual FilterResult filterContact(const Contact &contact);

	void setEnabled(bool enabled);

};

Q_DECLARE_METATYPE(HideOfflineWithoutDescriptionTalkableFilter *)

#endif // HIDE_OFFLINE_WITHOUT_DESCRIPTION_TALKABLE_FILTER_H
