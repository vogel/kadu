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

GaduPubdirSocketNotifiers::GaduPubdirSocketNotifiers(struct gg_http *h, QObject *parent)
	: GaduSocketNotifiers(h->fd, parent), H(h)
{
	kdebugf();
	kdebugf2();
}

GaduPubdirSocketNotifiers::~GaduPubdirSocketNotifiers()
{
	kdebugf();
	deleteSocketNotifiers();
	kdebugf2();
}

void GaduPubdirSocketNotifiers::dataReceived()
{
	kdebugf();

	Snr->setEnabled(false);

	if (H->check & GG_CHECK_READ)
		socketEvent();

	if(Snr) Snr->setEnabled(true);

	kdebugf2();
}

void GaduPubdirSocketNotifiers::dataSent()
{
	kdebugf();

	Snw->setEnabled(false);
	if (H->check & GG_CHECK_WRITE)
		socketEvent();

	kdebugf2();
}

void GaduPubdirSocketNotifiers::socketEvent()
{
	kdebugf();

	if (gg_pubdir_watch_fd(H) == -1)
	{
		deleteSocketNotifiers();
		emit done(false, H);
		gg_pubdir_free(H);
		deleteLater();
		return;
	}

	struct gg_pubdir *p = (struct gg_pubdir *)H->data;

	switch (H->state)
	{
		case GG_STATE_CONNECTING:
			kdebugmf(KDEBUG_NETWORK | KDEBUG_INFO, "changing QSocketNotifiers\n");
			recreateSocketNotifiers();

			if (H->check & GG_CHECK_WRITE)
				Snw->setEnabled(true);

			break;

		case GG_STATE_ERROR:
			kdebugmf(KDEBUG_NETWORK | KDEBUG_INFO, "error!\n");
			deleteSocketNotifiers();
			emit done(false, H);
			gg_pubdir_free(H);
			deleteLater();
			break;

		case GG_STATE_DONE:
			deleteSocketNotifiers();

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
				Snw->setEnabled(true);
	}
	kdebugf2();
}
