/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef GADU_CONTACT_LIST_SERVICE_H
#define GADU_CONTACT_LIST_SERVICE_H

#include "protocols/services/contact-list-service.h"

class GaduProtocol;
class GaduProtocolSocketNotifiers;

class GaduContactListService : public ContactListService
{
	Q_OBJECT

	GaduProtocol *Protocol;
	QByteArray ImportReply;

	friend class GaduProtocolSocketNotifiers;
	void handleEventUserlistGetReply(struct gg_event *e);
	void handleEventUserlistPutReply(struct gg_event *e);
	void handleEventUserlist(struct gg_event *e);

public:
	GaduContactListService(GaduProtocol *protocol);

	virtual void importContactList(bool automaticallySetBuddiesList = true);
	virtual void exportContactList();
	virtual void exportContactList(const BuddyList &buddies);

	virtual QList<Buddy> loadBuddyList(QTextStream &dataStream);
	virtual QByteArray storeBuddyList(const BuddyList &buddies);

};

#endif // GADU_CONTACT_LIST_SERVICE_H
