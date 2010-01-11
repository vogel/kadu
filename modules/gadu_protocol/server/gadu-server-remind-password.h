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

#ifndef GADU_SERVER_REMIND_PASSWORD_H
#define GADU_SERVER_REMIND_PASSWORD_H

#include <libgadu.h>

#include "protocols/protocol.h"

#include "gadu-server-connector.h"

class GaduServerRemindPassword : public GaduServerConnector
{
	Q_OBJECT

	struct gg_http *H;

	UinType Uin;
	QString Mail;

private slots:
	void done(bool ok, struct gg_http *h);

protected:
	virtual void performAction(const QString &tokenId, const QString &tokenValue);

public:
	GaduServerRemindPassword(TokenReader *reader, UinType uin, const QString &mail);

	UinType uin() { return Uin; }

};

#endif // GADU_SERVER_REMIND_PASSWORD_H
