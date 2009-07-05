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

#include <QtCore/QObject>

#include <libgadu.h>

#include "protocols/protocol.h"

class GaduServerRegisterAccount : public QObject
{
	Q_OBJECT

	struct gg_http *H;

	bool Result;

	UinType Uin;
	QString Mail;
	QString Password;
	QString TokenId;
	QString TokenValue;

private slots:
	void done(bool ok, struct gg_http *h);

public:
	GaduServerRegisterAccount(const QString &mail, const QString &password, const QString &tokenId, const QString &tokenValue);

	void performAction();

	UinType uin() { return Uin; }

	bool result() { return Result; }

signals:
	void finished(GaduServerRegisterAccount *);
};

#endif // GADU_SERVER_REGISTER_ACCOUNT_H
