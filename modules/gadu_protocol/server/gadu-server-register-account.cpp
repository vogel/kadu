/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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

#include "misc/misc.h"

#include "socket-notifiers/gadu-pubdir-socket-notifiers.h"

#include "gadu-server-register-account.h"

GaduServerRegisterAccount::GaduServerRegisterAccount(const QString &mail, const QString &password, const QString &tokenId, const QString &tokenValue) :
		QObject(), H(0), Result(0), Uin(0), Mail(mail), Password(password), TokenId(tokenId), TokenValue(tokenValue)
{
}

void GaduServerRegisterAccount::performAction()
{
	H = gg_register3(
			Mail.toUtf8().constData(),
			Password.toUtf8().constData(),
			TokenId.toUtf8().constData(),
			TokenValue.toUtf8().constData(),
			true);

	if (H && H->fd > 0)
	{
		GaduPubdirSocketNotifiers *sn = new GaduPubdirSocketNotifiers();
		connect(sn, SIGNAL(done(bool, struct gg_http *)), this, SLOT(done(bool, struct gg_http *)));
		sn->watchFor(H);
	}
	else
		emit finished(this);
}

void GaduServerRegisterAccount::done(bool ok, struct gg_http *h)
{
	if (ok)
		Uin = ((struct gg_pubdir *)h->data)->uin;

	Result = ok;

	if (H)
	{
		delete H;
		H = 0;
	}

	emit finished(this);
}
