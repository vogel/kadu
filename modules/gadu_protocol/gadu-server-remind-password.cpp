/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "misc.h"

#include "socket-notifiers/gadu-pubdir-socket-notifiers.h"

#include "gadu-server-remind-password.h"

GaduServerRemindPassword::GaduServerRemindPassword(TokenReader *reader, UinType uin, const QString &mail)
	: GaduServerConnector(reader), Uin(uin), Mail(mail)
{
}

void GaduServerRemindPassword::performAction(const QString &tokenId, const QString &tokenValue)
{
	H = gg_remind_passwd3(Uin, unicode2cp(Mail).data(), unicode2cp(tokenId).data(),
		unicode2cp(tokenValue).data(), 1);
	if (H)
	{
		GaduPubdirSocketNotifiers *sn = new GaduPubdirSocketNotifiers();
		connect(sn, SIGNAL(done(bool, struct gg_http *)), this, SLOT(done(bool, struct gg_http *)));
		sn->watchFor(H);
	}
	else
		finished(false);
}

void GaduServerRemindPassword::done(bool ok, struct gg_http *h)
{
	finished(ok);

	if (H)
	{
		delete H;
		H = 0;
	}
}
