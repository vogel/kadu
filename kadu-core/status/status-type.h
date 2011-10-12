/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
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

#ifndef STATUS_TYPE_H
#define STATUS_TYPE_H

#include <QtCore/QVariant>

enum StatusType
{
	StatusTypeNone = -1,
	StatusTypeFreeForChat = 0,
	StatusTypeOnline = 10,
	StatusTypeAway = 20,
	StatusTypeNotAvailable = 30,
	StatusTypeDoNotDisturb = 40,
	StatusTypeInvisible = 50,
	StatusTypeOffline = 60
};

Q_DECLARE_METATYPE(StatusType)

#endif // STATUS_TYPE_H
