/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#ifndef ACTION_CONTEXT_H
#define ACTION_CONTEXT_H

#include <QtCore/QObject>

#include "model/roles.h"

class BuddySet;
class Chat;
class ContactSet;
class StatusContainer;

/**
 * @addtogroup Actions
 * @{
 */

/**
 * @class ActionContext
 * @author Rafał 'Vogel' Malinowski
 * @short Interface responsible for storing data required for proper Action behavior.
 *
 * This inteface is used by all actions classes to get information required for proper Action behavior.
 * Each action in Kadu can easily get information about widgets and window, where they were used. Data
 * about contacts, buddies, chats and status containers is transfered using this interface. Each window
 * or widgets that can contain action must implement this interface.
 *
 * Data for each ActionContext may be different depending on current state of window - like selection
 * of items in buddies list view, or may be constant - like list of buddies in classic chat window.
 */
class ActionContext : public QObject
{
	Q_OBJECT

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns contact set for given action data source.
	 *
	 * Returns contact set for given action data source. May return empty contact set.
	 */
	virtual ContactSet contacts() = 0;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns buddy set for given action data source.
	 *
	 * Returns buddy set for given action data source. May return empty buddy set.
	 */
	virtual BuddySet buddies() = 0;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns chat for given action data source.
	 *
	 * Returns chat given action data source. May return empty chat.
	 */
	virtual Chat chat() = 0;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns status container for given action data source.
	 *
	 * Returns status container for given action data source. May return null.
	 */
	virtual StatusContainer * statusContainer() = 0;

	virtual RoleSet roles() = 0;

signals:
	void changed();

};

/**
 * @}
 */

#endif // ACTION_CONTEXT_H
