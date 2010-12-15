/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include <libgadu.h>

#include <QtCore/QByteArray>

#include "buddies/buddy-manager.h"

#include "debug.h"
#include "misc/misc.h"

#include "../helpers/gadu-list-helper.h"

#include "../gadu-protocol.h"
#include "socket-notifiers/gadu-protocol-socket-notifiers.h"

#include "gadu-contact-list-service.h"

GaduContactListService::GaduContactListService(GaduProtocol *protocol) :
		ContactListService(protocol), Protocol(protocol)
{
}

void GaduContactListService::handleEventUserlistGetReply(struct gg_event *e)
{
	char *content = e->event.userlist.reply;
	if (!content)
	{
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "error!\n");

		emit contactListImported(false, BuddyList());
		return;
	}

	if (content[0] != 0)
		ImportReply.append(content);

	if (e->event.userlist.type == GG_USERLIST_GET_MORE_REPLY)
	{
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "next portion\n");
		return;
	}

	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "\n%s\n", ImportReply.data());

	emit contactListImported(true, GaduListHelper::byteArrayToBuddyList(Protocol->account(), ImportReply));
}

void GaduContactListService::handleEventUserlistPutReply(struct gg_event *e)
{
	Q_UNUSED(e)

	emit contactListExported(true);
	return;
}

void GaduContactListService::handleEventUserlist(struct gg_event *e)
{
	switch (e->event.userlist.type)
	{
		case GG_USERLIST_GET_REPLY:
		case GG_USERLIST_GET_MORE_REPLY:
			handleEventUserlistGetReply(e);
			break;
		case GG_USERLIST_PUT_REPLY:
			handleEventUserlistPutReply(e);
			break;
	}
}

void GaduContactListService::importContactList()
{
	ImportReply.clear();

	if (-1 == gg_userlist_request(Protocol->gaduSession(), GG_USERLIST_GET, 0))
		emit contactListImported(false, BuddyList());
}

void GaduContactListService::exportContactList()
{
	exportContactList(BuddyManager::instance()->buddies(Protocol->account()));
}

void GaduContactListService::exportContactList(const BuddyList &buddies)
{
	QByteArray contacts = GaduListHelper::buddyListToByteArray(Protocol->account(), buddies);

	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "\n%s\n", contacts.constData());

	if (-1 == gg_userlist_request(Protocol->gaduSession(), GG_USERLIST_PUT, contacts.constData()))
		emit contactListExported(false);
}

QList<Buddy> GaduContactListService::loadBuddyList(QTextStream &dataStream)
{
	return GaduListHelper::streamToBuddyList(Protocol->account(), dataStream);
}

QByteArray GaduContactListService::storeBuddyList(const BuddyList &buddies)
{
	return GaduListHelper::buddyListToByteArray(Protocol->account(), buddies);
}
