/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "socket-notifiers/gadu-token-socket-notifiers.h"

#include "gadu-server-connector.h"

void GaduServerConnector::finished(bool result)
{
	Result = result;
	emit finished(this);
}

void GaduServerConnector::perform()
{
	H = gg_token(1);
	if (!H || H->fd <= 0)
	{
		tokenFetchFailed();
		return;
	}

	GaduTokenSocketNotifiers *sn = new GaduTokenSocketNotifiers();
	connect(sn, SIGNAL(tokenError()), this, SLOT(tokenFetchFailed()));
	connect(sn, SIGNAL(gotToken(const QString &, const QPixmap &)),
			this, SLOT(tokenFetched(const QString &, const QPixmap &)));
	sn->watchFor(H);
}

void GaduServerConnector::tokenFetchFailed()
{
	finished(false);

	if (H)
	{
		delete H;
		H = 0;
	}
}

void GaduServerConnector::tokenFetched(const QString &tokenId, const QPixmap &tokenPixmap)
{
	if (Reader)
	{
		QString tokenValue = Reader->readToken(tokenPixmap);
		performAction(tokenId, tokenValue);
	}
	else
		finished(false);

	if (H)
	{
		delete H;
		H = 0;
	}
}
