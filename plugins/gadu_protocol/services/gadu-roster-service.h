/*
 * %kadu copyright begin%
 * Copyright 2008, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
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

#ifndef GADU_USERLIST_HANDLER_H
#define GADU_USERLIST_HANDLER_H

#include <libgadu.h>

#include "protocols/services/roster-service.h"

class GaduRosterService : public RosterService
{
	Q_OBJECT

	gg_session *GaduSession;

	void updateFlag(int uin, int newFlags, int oldFlags, int flag) const;
	void sendNewFlags(const Contact &contact, int newFlags) const;

protected slots:
	virtual void updateContact(const Contact &contact);

public:
	static int notifyTypeFromContact(const Contact &contact);

	explicit GaduRosterService(Protocol *protocol);
	virtual ~GaduRosterService();

	virtual void prepareRoster();

public slots:
	void setGaduSession(gg_session *gaduSession);

	virtual bool addContact(const Contact &contact);
	virtual bool removeContact(const Contact &contact);

};

#endif // GADU_USERLIST_HANDLER_H
