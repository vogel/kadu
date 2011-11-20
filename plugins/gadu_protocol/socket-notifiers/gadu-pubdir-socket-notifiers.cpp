/*
 * %kadu copyright begin%
 * Copyright 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

void GaduPubdirSocketNotifiers::finished(bool ok)
{
	emit done(ok, H);
	watchFor(0);
	deleteLater();
}

void GaduPubdirSocketNotifiers::socketEvent()
{
	kdebugf();

	if (gg_pubdir_watch_fd(H) == -1)
	{
		finished(false);
		return;
	}

	struct gg_pubdir *p = (struct gg_pubdir *)H->data;

	switch (H->state)
	{
		case GG_STATE_CONNECTING:
			kdebugmf(KDEBUG_NETWORK | KDEBUG_INFO, "changing QSocketNotifiers\n");
			watchFor(H);
			break;

		case GG_STATE_ERROR:
			kdebugmf(KDEBUG_NETWORK | KDEBUG_INFO, "error!\n");
			finished(false);
			break;

		case GG_STATE_DONE:
			if (p->success)
			{
				kdebugmf(KDEBUG_NETWORK | KDEBUG_INFO,  "success!\n");
			}
			else
			{
				kdebugmf(KDEBUG_NETWORK | KDEBUG_INFO, "error!\n");
			}
			finished(p->success);
			break;
	}

	kdebugf2();
}

int GaduPubdirSocketNotifiers::timeout()
{
	return H
		? H->timeout * 1000
		: 0;
}

bool GaduPubdirSocketNotifiers::handleSoftTimeout()
{
	return false;
}

void GaduPubdirSocketNotifiers::connectionTimeout()
{
	finished(false);
}
