/*
 * %kadu copyright begin%
 * Copyright 2008, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2009, 2010 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef ROLES_H
#define ROLES_H

#include <QtCore/QMetaType>
#include <QtCore/QSet>
#include <QtWidgets/QAction>

const int KaduRoles = 1000;

const int BuddyRole = KaduRoles + 1;
const int DescriptionRole = KaduRoles + 2;
const int StatusRole = KaduRoles + 3;
const int ProtocolRole = KaduRoles + 4;
const int AccountRole = KaduRoles + 5;
const int AvatarRole = KaduRoles + 6;
const int ChatRole = KaduRoles + 7;
const int ChatTypeRole = KaduRoles + 8;
const int DateRole = KaduRoles + 9;
const int ActionRole = KaduRoles + 10;
const int ContactRole = KaduRoles + 11;
const int BuddyContactsTableItemRole = KaduRoles + 12;
const int GroupRole = KaduRoles + 13;
const int IdentityRole = KaduRoles + 14;
const int ItemTypeRole = KaduRoles + 15;
const int MultilogonSessionRole = KaduRoles + 16;
const int NetworkProxyRole = KaduRoles + 17;
const int TalkableRole = KaduRoles + 18;
const int AvatarPathRole = KaduRoles + 19;
const int StatusIconPath = KaduRoles + 20;

typedef QSet<int> RoleSet;
// needed for ActionRole
Q_DECLARE_METATYPE(QAction *);

#endif // ROLES_H
