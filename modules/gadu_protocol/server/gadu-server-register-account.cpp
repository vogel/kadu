/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "misc/misc.h"

#include "socket-notifiers/gadu-pubdir-socket-notifiers.h"

#include "gadu-server-register-account.h"

GaduServerRegisterAccount::GaduServerRegisterAccount(const QString &mail, const QString &password, const QString &tokenId, const QString &tokenValue)
	: QObject(), H(0), Uin(0), Result(0), Mail(mail), Password(password), TokenId(tokenId), TokenValue(tokenValue)
{
}

void GaduServerRegisterAccount::performAction()
{
	H = gg_register3(unicode2cp(Mail).data(), unicode2cp(Password).data(),
			unicode2cp(TokenId).data(), unicode2cp(TokenValue).data(), 1);
	if (H)
	{
		GaduPubdirSocketNotifiers *sn = new GaduPubdirSocketNotifiers();
		connect(sn, SIGNAL(done(bool, struct gg_http *)), this, SLOT(done(bool, struct gg_http *)));
		sn->watchFor(H);
	}
	else
		finished(this);
}

void GaduServerRegisterAccount::done(bool ok, struct gg_http *h)
{
	if (ok)
		Uin = ((struct gg_pubdir *)h->data)->uin;

	Result = ok;

	finished(this);

	if (H)
	{
		delete H;
		H = 0;
	}
}
