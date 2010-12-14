/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef SMS_SENDER_H
#define SMS_SENDER_H

#include <QtCore/QObject>

#include "misc/token-acceptor.h"

class TokenReader;

class SmsSender : public QObject, public TokenAcceptor
{
	Q_OBJECT

	QString Number;
	QString Signature;

	TokenReader *MyTokenReader;

	void fixNumber();

protected:
	QString Message;

	bool validateNumber();
	bool validateSignature();

public:
	explicit SmsSender(const QString &number, QObject *parent = 0);
	virtual ~SmsSender();

	const QString & number() const { return Number; }
	const QString & signature() const { return Signature; }
	TokenReader * tokenReader() { return MyTokenReader; }

	void setSignature(const QString &signature);

	virtual void sendMessage(const QString &message) = 0;

	void setTokenReader(TokenReader *tokenReader);

	virtual void tokenRead(const QString& tokenValue);

signals:
	void gatewayAssigned(const QString &number, const QString &gatewayId);
	void succeed(const QString &message);
	void failed(const QString &errorMessage);
};

#endif // SMS_SENDER_H
