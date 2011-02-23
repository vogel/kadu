/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef GTALK_PROTOCOL_FACTORY_H
#define GTALK_PROTOCOL_FACTORY_H

#include "jabber-protocol-factory.h"

class GTalkProtocolFactory : public JabberProtocolFactory
{
	Q_OBJECT
	Q_DISABLE_COPY(GTalkProtocolFactory)

	static GTalkProtocolFactory *Instance;

	GTalkProtocolFactory();

public:
	static void createInstance();
	static void destroyInstance();

	static GTalkProtocolFactory * instance() { return Instance; }

	virtual QString idLabel();
    virtual QString defaultServer();

	virtual QString name() { return "gmail/google talk"; }
	virtual QString displayName() { return "Gmail/Google Talk"; }

	virtual bool canRegister() { return false; }

	virtual QIcon icon();
	virtual QString iconPath();

	virtual AccountCreateWidget * newCreateAccountWidget(QWidget */*parent*/ = 0) { return 0; }

};

#endif // GTALK_PROTOCOL_FACTORY_H
