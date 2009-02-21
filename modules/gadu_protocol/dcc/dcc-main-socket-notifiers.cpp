/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "debug.h"

#include "dcc-socket.h"

#include "dcc-main-socket-notifiers.h"

void DccMainSocketNotifiers::handleEvent(struct gg_event *e)
{
	switch (e->type)
	{
		case GG_EVENT_DCC_NEW:
		{
			kdebugmf(KDEBUG_NETWORK | KDEBUG_INFO, "GG_EVENT_DCC_NEW\n");

			DccSocket *dccSocket = new DccSocket(manager(), e->event.dcc_new);
// 			dccSocket->setHandler(this);
			break;
		}

		default:
			DccMainSocketNotifiers::handleEvent(e);
			break;
	}

}
