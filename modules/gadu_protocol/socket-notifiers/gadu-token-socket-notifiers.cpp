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
#include "misc.h"

#include "gadu-token-socket-notifiers.h"

GaduTokenSocketNotifiers::GaduTokenSocketNotifiers(QObject *parent)
	: GaduSocketNotifiers(0, parent), H(0)
{
	kdebugf();
	kdebugf2();
}

GaduTokenSocketNotifiers::~GaduTokenSocketNotifiers()
{
	kdebugf();
	deleteSocketNotifiers();
	kdebugf2();
}

void GaduTokenSocketNotifiers::start()
{
	kdebugf();

	if (!(H = gg_token(1)))
	{
		emit tokenError();
		return;
	}

	if (!H->fd)
	{
		emit tokenError();
		return;
	}

	setSocket(H->fd);
	createSocketNotifiers();
	kdebugf2();
}

bool GaduTokenSocketNotifiers::checkRead()
{
	return H && (H->check & GG_CHECK_READ);
}

bool GaduTokenSocketNotifiers::checkWrite()
{
	return H && (H->check & GG_CHECK_WRITE);
}

void GaduTokenSocketNotifiers::socketEvent()
{
	kdebugf();

	if (gg_token_watch_fd(H) == -1)
	{
		deleteSocketNotifiers();
		emit tokenError();
		gg_token_free(H);
		H = NULL;
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "getting token error\n");
		deleteLater();
		return;
	}

	struct gg_pubdir *p = (struct gg_pubdir *)H->data;

	switch (H->state)
	{

		case GG_STATE_CONNECTING:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "changing QSocketNotifiers.\n");
			deleteSocketNotifiers();
			createSocketNotifiers();
			if (H->check & GG_CHECK_WRITE)
				setWriteEnabled(true);
			break;

		case GG_STATE_ERROR:
			deleteSocketNotifiers();
			emit tokenError();
			gg_token_free(H);
			H = NULL;
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "getting token error\n");
			deleteLater();
			break;

		case GG_STATE_DONE:
			deleteSocketNotifiers();
			if (p->success)
			{
				kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "success\n");

				struct gg_token *t = (struct gg_token *)H->data;
				QString tokenId = cp2unicode(t->tokenid);

				//nie optymalizowac!!!
				QByteArray buf(H->body_size, '0');
				for (unsigned int i = 0; i < H->body_size; ++i)
					buf[i] = H->body[i];

				QPixmap tokenImage;
				tokenImage.loadFromData(buf);

				emit gotToken(tokenId, tokenImage);
			}

			else
			{
				kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "getting token error\n");
				emit tokenError();
			}

			gg_token_free(H);
			H = NULL;
			deleteLater();
			break;

		default:
			if (H->check & GG_CHECK_WRITE)
				setWriteEnabled(true);
	}

	kdebugf2();
}
