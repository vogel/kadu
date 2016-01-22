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

#pragma once

#include "talkable/filter/talkable-filter.h"

#include <QtCore/QMetaType>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class BuddyManager;
class BuddyPreferredManager;
class ContactManager;

/**
 * @addtogroup Talkable
 * @{
 */

/**
 * @class HideOfflineTalkableFilter
 * @author Rafał 'Vogel' Malinowski
 * @short Filter that removes items that are not available.
 *
 * This filter removes items that are not available. Contact items that have offline status are rejected.
 * Buddy items that have preffered contact with offline status are rejected. All other items are passed
 * to next filters.
 *
 * This filter can be enable or disabled. In disabled state it always returs Undecided.
 */
class HideOfflineTalkableFilter : public TalkableFilter
{
	Q_OBJECT

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Create new instance of HideOfflineTalkableFilter with given parent.
	 * @param parent QObject parent of new object
	 */
	explicit HideOfflineTalkableFilter(QObject *parent = nullptr);
	virtual ~HideOfflineTalkableFilter();

	virtual FilterResult filterBuddy(const Buddy &buddy);
	virtual FilterResult filterContact(const Contact &contact);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Enable or disalbe filter.
	 * @param enabled new value of enabled property
	 */
	void setEnabled(bool enabled);

private:
	QPointer<BuddyManager> m_buddyManager;
	QPointer<BuddyPreferredManager> m_buddyPreferredManager;
	QPointer<ContactManager> m_contactManager;

	bool m_enabled;

private slots:
	INJEQT_SET void setBuddyManager(BuddyManager *buddyManager);
	INJEQT_SET void setBuddyPreferredManager(BuddyPreferredManager *buddyPreferredManager);
	INJEQT_SET void setContactManager(ContactManager *contactManager);

};

/**
 * @}
 */

Q_DECLARE_METATYPE(HideOfflineTalkableFilter *)
