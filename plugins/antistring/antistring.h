/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef ANTISTRING_H
#define ANTISTRING_H

#include "configuration/configuration-aware-object.h"
#include "gui/windows/main-configuration-window.h"
#include "message/message-filter.h"

#include "antistring-configuration.h"

class Account;
class Contact;

class Antistring : public MessageFilter
{
	Q_OBJECT
	Q_DISABLE_COPY(Antistring)

	static Antistring * Instance;

	AntistringConfiguration Configuration;

	Antistring();
	virtual ~Antistring();

	int points(const QString &message);
	void writeLog(Contact sender, const QString &message);

public:
	static void createInstance();
	static void destroyInstance();

	static Antistring * instance() { return Instance; }

	AntistringConfiguration & configuration() { return Configuration; }

	virtual bool acceptMessage(const Message &message);

};

#endif // ANTISTRING_H
