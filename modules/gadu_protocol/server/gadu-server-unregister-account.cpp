/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@go2.pl)
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

#include "gadu-server-unregister-account.h"

GaduServerUnregisterAccount::GaduServerUnregisterAccount(TokenReader *reader, UinType uin, const QString &password)
	: GaduServerConnector(reader), Uin(uin), Password(password)
{
}

void GaduServerUnregisterAccount::performAction(const QString &tokenId, const QString &tokenValue)
{
	H = gg_unregister3(Uin, Password.toUtf8().constData(), tokenId.toUtf8().constData(),
		tokenValue.toUtf8().constData(), 1);
	if (H)
	{
		GaduPubdirSocketNotifiers *sn = new GaduPubdirSocketNotifiers();
		connect(sn, SIGNAL(done(bool, struct gg_http *)),
			this, SLOT(done(bool, struct gg_http *)));
		sn->watchFor(H);
	}
	else
		finished(false);
}

void GaduServerUnregisterAccount::done(bool ok, struct gg_http *h)
{
	Q_UNUSED(h)

	finished(ok);

	if (H)
	{
		delete H;
		H = 0;
	}
}
