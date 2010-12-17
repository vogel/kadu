/*
 * %kadu copyright begin%
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
		emit tokenFetched(QString(), QPixmap());
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
