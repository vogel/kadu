/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

/*
 * jidutil.h
 * Copyright (C) 2006  Remko Troncon
 */

#ifndef JIDUTIL
#define JIDUTIL

class QString;
namespace XMPP {
	class Jid;
}

class JIDUtil
{
public:
// 	static QString defaultDomain();
// 	static void setDefaultDomain(QString domain);

	static QString accountToString(const XMPP::Jid&, bool withResource);
	static XMPP::Jid accountFromString(const QString&);
	static QString toString(const XMPP::Jid&, bool withResource);
	static XMPP::Jid fromString(const QString&);
	static QString encode(const QString &jid);
	static QString decode(const QString &jid);
	static QString nickOrJid(const QString&, const QString&);

	static QString encode822(const QString&);
	static QString decode822(const QString&);
};

#endif
