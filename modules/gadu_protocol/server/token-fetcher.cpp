/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <libgadu.h>

#include "socket-notifiers/gadu-token-socket-notifiers.h"

#include "token-fetcher.h"

TokenFetcher::TokenFetcher(bool autoDelete, QObject *parent) :
		QObject(parent), H(0), AutoDelete(autoDelete)
{
}

TokenFetcher::~TokenFetcher()
{
	delete H;
	H = 0;
}

void TokenFetcher::fetchToken()
{
	delete H;

	H = gg_token(1);
	if (!H || H->fd <= 0)
	{
		emit tokenFetched(QString::null, QPixmap());
		return;
	}

	GaduTokenSocketNotifiers *sn = new GaduTokenSocketNotifiers();
	connect(sn, SIGNAL(done(const QString &, const QPixmap &)),
			this, SLOT(tokenFetchedSlot(const QString &, const QPixmap &)));
	sn->watchFor(H);
}

void TokenFetcher::tokenFetchedSlot(const QString &tokenId, QPixmap tokenPixmap)
{
	delete H;
	H = 0;

	emit tokenFetched(tokenId, tokenPixmap);

	if (AutoDelete)
		deleteLater();
}
