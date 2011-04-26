/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include "gadu-server-change-password.h"

GaduServerChangePassword::GaduServerChangePassword(UinType uin,	const QString &mail, const QString &password,
		const QString &newPassword, const QString &tokenId, const QString &tokenValue) :
	QObject(), H(0), Uin(uin), Mail(mail), Password(password), NewPassword(newPassword), TokenId(tokenId), TokenValue(tokenValue)
{
}

void GaduServerChangePassword::performAction()
{
	H = gg_change_passwd4(Uin,
			Mail.toUtf8().constData(),
			Password.toUtf8().constData(),
			NewPassword.toUtf8().constData(),
			TokenId.toUtf8().constData(),
			TokenValue.toUtf8().constData(),
			false);

	if (H)
	{
		struct gg_pubdir *result = (struct gg_pubdir *)H->data;
		Result = result->success;

		if (result->success)
			Uin = result->uin;

		emit finished(this);
	}

	/* TODO: fix for 0.6.6, it should be async
	if (H && H->fd > 0)
	{
		GaduPubdirSocketNotifiers *sn = new GaduPubdirSocketNotifiers();
		connect(sn, SIGNAL(done(bool, struct gg_http *)), this, SLOT(done(bool, struct gg_http *)));
		sn->watchFor(H);
	}
	else
		emit finished(this);*/
}

void GaduServerChangePassword::done(bool ok, struct gg_http *h)
{
	if (ok)
		Uin = ((struct gg_pubdir *)h->data)->uin;

	Result = ok;

	emit finished(this);

	if (H)
	{
		delete H;
		H = 0;
	}
}
