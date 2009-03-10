/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <libgadu.h>

#include <QtCore/QByteArray>

#include "contacts/contact-manager.h"

#include "debug.h"
#include "misc.h"

#include "../helpers/gadu-list-helper.h"

#include "../gadu-protocol.h"
#include "socket-notifiers/gadu-protocol-socket-notifiers.h"

#include "gadu-contact-list-service.h"

GaduContactListService::GaduContactListService(GaduProtocol *protocol)
	: ContactListService(protocol), Protocol(protocol)
{
}

void GaduContactListService::handleEventUserlistGetReply(struct gg_event *e)
{
	char *content = e->event.userlist.reply;
	if (!content)
	{
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "error!\n");

		emit contactListImported(false, ContactList());
		return;
	}

	if (content[0] != 0)
		ImportReply += cp2unicode(content);

	if (e->event.userlist.type == GG_USERLIST_GET_MORE_REPLY)
	{
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "next portion\n");
		return;
	}

	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "\n%s\n", unicode2latin(ImportReply).data());

	emit contactListImported(true, GaduListHelper::stringToContactList(Protocol->account(), ImportReply));
}

void GaduContactListService::handleEventUserlistPutReply(struct gg_event *e)
{
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
	ImportReply.truncate(0);

	if (-1 == gg_userlist_request(Protocol->gaduSession(), GG_USERLIST_GET, 0) != -1)
		emit contactListImported(false, ContactList());
}

void GaduContactListService::exportContactList()
{
	exportContactList(ContactManager::instance()->contacts(Protocol->account()));
}

void GaduContactListService::exportContactList(ContactList contacts)
{
	QString contactsString = GaduListHelper::contactListToString(Protocol->account(), contacts);

	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "\n%s\n", unicode2cp(contactsString).data());

	if (-1 == gg_userlist_request(Protocol->gaduSession(), GG_USERLIST_PUT, unicode2cp(contactsString)))
		emit contactListExported(false);
}
