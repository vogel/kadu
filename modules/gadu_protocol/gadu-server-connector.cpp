/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "gadu-token-socket-notifiers.h"

#include "gadu-server-connector.h"

void GaduServerConnector::perform()
{
	GaduTokenSocketNotifiers *sn = new GaduTokenSocketNotifiers(this);
	connect(sn, SIGNAL(tokenError()), this, SLOT(tokenError()));
	connect(sn, SIGNAL(gotToken(const QString &, const QPixmap &)),
			this, SLOT(tokenFetched(const QString &, const QPixmap &)));
	sn->start();
}

void GaduServerConnector::tokenFetched(const QString &tokenId, const QPixmap &tokenPixmap)
{
	if (!Reader)
	{
		emit result(false);
		return;
	}

	QString tokenValue = Reader->readToken(tokenPixmap);
	performAction(tokenId, tokenValue);
}
