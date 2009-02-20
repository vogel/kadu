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

#include "gadu-server-change-password.h"

GaduServerChangePassword::GaduServerChangePassword(TokenReader *reader, UinType uin,
		const QString &mail, const QString &password, const QString &newPassword) :
	GaduServerConnector(reader), H(0), Uin(uin), Mail(mail), Password(password), NewPassword(newPassword)
{
}

void GaduServerChangePassword::performAction(const QString &tokenId, const QString &tokenValue)
{
	H = gg_change_passwd4(Uin, unicode2cp(Mail).data(),
			unicode2cp(Password).data(), unicode2cp(NewPassword).data(),
			unicode2cp(tokenId).data(), unicode2cp(tokenValue).data(), 1);
	if (H)
	{
		GaduPubdirSocketNotifiers *sn = new GaduPubdirSocketNotifiers();
		connect(sn, SIGNAL(done(bool, struct gg_http *)), this,
			SLOT(done(bool, struct gg_http *)));
		sn->watchFor(H);
	}
	else
		finished(false);
}

void GaduServerChangePassword::done(bool ok, struct gg_http *h)
{
	finished(ok);

	if (H)
	{
		delete H;
		H = 0;
	}
}
