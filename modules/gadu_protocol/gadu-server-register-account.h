/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GADU_SERVER_REGISTER_ACCOUNT_H
#define GADU_SERVER_REGISTER_ACCOUNT_H

#include <libgadu.h>

#include "protocols/protocol.h"

#include "gadu-server-connector.h"

class GaduServerRegisterAccount : public GaduServerConnector
{
	Q_OBJECT

	struct gg_http *H;

	UinType Uin;
	QString Mail;
	QString Password;

private slots:
	void done(bool ok, struct gg_http *h);

protected:
	virtual void performAction(const QString &tokenId, const QString &tokenValue);

public:
	GaduServerRegisterAccount(TokenReader *reader, const QString &mail, const QString &password);

	UinType uin() { return Uin; }

};

#endif // GADU_SERVER_REGISTER_ACCOUNT_H
