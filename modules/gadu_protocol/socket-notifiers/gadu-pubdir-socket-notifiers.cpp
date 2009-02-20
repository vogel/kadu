/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QSocketNotifier>

#include <libgadu.h>

#include "debug.h"

#include "gadu-pubdir-socket-notifiers.h"

void GaduPubdirSocketNotifiers::watchFor(struct gg_http *h)
{
	H = h;
	GaduSocketNotifiers::watchFor(H ? H->fd : 0);
}

bool GaduPubdirSocketNotifiers::checkRead()
{
	return H && (H->check & GG_CHECK_READ);
}

bool GaduPubdirSocketNotifiers::checkWrite()
{
	return H && (H->check & GG_CHECK_WRITE);
}

void GaduPubdirSocketNotifiers::socketEvent()
{
	kdebugf();

	if (gg_pubdir_watch_fd(H) == -1)
	{
		emit done(false, H);
		watchFor(0);
		deleteLater();
		return;
	}

	struct gg_pubdir *p = (struct gg_pubdir *)H->data;

	switch (H->state)
	{
		case GG_STATE_CONNECTING:
			kdebugmf(KDEBUG_NETWORK | KDEBUG_INFO, "changing QSocketNotifiers\n");
			watchFor(H);

			if (H->check & GG_CHECK_WRITE)
				setWriteEnabled(true);

			break;

		case GG_STATE_ERROR:
			kdebugmf(KDEBUG_NETWORK | KDEBUG_INFO, "error!\n");
			watchFor(0);

			emit done(false, H);
			gg_pubdir_free(H);
			deleteLater();
			break;

		case GG_STATE_DONE:
			watchFor(0);

			if (p->success)
			{
				kdebugmf(KDEBUG_NETWORK | KDEBUG_INFO, "success!\n");
				emit done(true, H);
			}
			else
			{
				kdebugmf(KDEBUG_NETWORK | KDEBUG_INFO, "error!\n");
				emit done(false, H);
			}
			gg_pubdir_free(H);
			deleteLater();
			break;

		default:
			if (H->check & GG_CHECK_WRITE)
				setWriteEnabled(true);
	}
	kdebugf2();
}
