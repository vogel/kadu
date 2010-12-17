/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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
#include "misc/misc.h"

#include "gadu-token-socket-notifiers.h"

void GaduTokenSocketNotifiers::watchFor(struct gg_http *h)
{
	H = h;
	GaduSocketNotifiers::watchFor(H ? H->fd : 0);
}

bool GaduTokenSocketNotifiers::checkRead()
{
	return H && (H->check & GG_CHECK_READ);
}

bool GaduTokenSocketNotifiers::checkWrite()
{
	return H && (H->check & GG_CHECK_WRITE);
}

void GaduTokenSocketNotifiers::finished(const QString& tokenId, const QPixmap& tokenPixmap)
{
	emit done(tokenId, tokenPixmap);
	watchFor(0);
	deleteLater();
}

void GaduTokenSocketNotifiers::socketEvent()
{
	kdebugf();

	if (gg_token_watch_fd(H) == -1)
	{
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "getting token error\n");
		finished(QString(), QPixmap());
		return;
	}

	struct gg_pubdir *p = (struct gg_pubdir *)H->data;

	switch (H->state)
	{

		case GG_STATE_CONNECTING:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "changing QSocketNotifiers.\n");
			watchFor(H);
			break;

		case GG_STATE_ERROR:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "getting token error\n");
			finished(QString(), QPixmap());
			break;

		case GG_STATE_DONE:
			if (p->success)
			{
				kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "success\n");

				struct gg_token *t = (struct gg_token *)H->data;
				QString tokenId = QString::fromUtf8(t->tokenid);

				//nie optymalizowac!!!
				QByteArray buf(H->body_size, '0');
				for (unsigned int i = 0; i < H->body_size; ++i)
					buf[i] = H->body[i];

				QPixmap tokenImage;
				tokenImage.loadFromData(buf);

				finished(tokenId, tokenImage);
			}
			else
			{
				kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "getting token error\n");
				finished(QString(), QPixmap());
			}

			break;
	}

	kdebugf2();
}

int GaduTokenSocketNotifiers::timeout()
{
	return H
		? H->timeout * 1000
		: 0;
}

bool GaduTokenSocketNotifiers::handleSoftTimeout()
{
	return false;
}

void GaduTokenSocketNotifiers::connectionTimeout()
{
	finished(QString(), QPixmap());
}
