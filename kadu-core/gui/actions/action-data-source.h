/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef ACTION_DATA_SOURCE_H
#define ACTION_DATA_SOURCE_H

class BuddySet;
class Chat;
class ContactSet;

class ActionDataSource
{
public:
	virtual ContactSet contacts() = 0;
	virtual BuddySet buddies() = 0;
	virtual Chat chat() = 0;

	// for now this is a hack for "delete buddy" acction
	// i don't know real solution, but this is ok now
	virtual bool hasContactSelected() = 0;

};

#endif // ACTION_DATA_SOURCE_H
