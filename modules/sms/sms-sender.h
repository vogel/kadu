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

class SmsSender : public QObject
{
	Q_OBJECT

	QString Number;
	QString Contact;
	QString Signature;

	void fixNumber();

protected:
	bool validateNumber();
	bool validateSignature();

public:
	explicit SmsSender(const QString &number, QObject *parent = 0);
	virtual ~SmsSender();

	QString number() { return Number; }
	QString contact() { return Contact; }
	QString signature() { return Signature; }

	void setContact(const QString &contact);
	void setSignature(const QString &signature);
	void sendMessage(const QString &message);

signals:
	void finished(const QString &errorMessage);

};

#endif // SMS_SENDER_H
