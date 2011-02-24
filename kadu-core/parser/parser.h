/*
 * %kadu copyright begin%
 * Copyright 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2008, 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
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

#ifndef PARSER_H
#define PARSER_H

#include <QtCore/QMap>

#include "buddies/buddy-or-contact.h"

#include "exports.h"

class KADUAPI Parser
{
	typedef QString (*ObjectTagCallback)(const QObject * const);
	typedef QString (*BuddyOrContactTagCallback)(BuddyOrContact);

	static QMap<QString, BuddyOrContactTagCallback> registeredTags;
	static QMap<QString, ObjectTagCallback> registeredObjectTags;

	static QString executeCmd(const QString &cmd);

public:
	static QMap<QString, QString> globalVariables;
	static QString parse(const QString &s, const QObject * const object, bool escape = true);
	static QString parse(const QString &s, BuddyOrContact buddyOrContact, bool escape = true);
	static QString parse(const QString &s, BuddyOrContact buddyOrContact, const QObject * const object, bool escape = true);
	static bool registerTag(const QString &name, BuddyOrContactTagCallback);
	static bool unregisterTag(const QString &name, BuddyOrContactTagCallback);

	static bool registerObjectTag(const QString &name, ObjectTagCallback);
	static bool unregisterObjectTag(const QString &name, ObjectTagCallback);

};

#endif // PARSER_H
