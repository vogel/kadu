/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2007, 2008, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
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
	QString Signature;

	void fixNumber();

protected:
	QString Message;

	bool validateSignature();

public:
	explicit SmsSender(const QString &number, QObject *parent = 0);
	virtual ~SmsSender();

	const QString & number() const { return Number; }
	const QString & signature() const { return Signature; }

	void setSignature(const QString &signature);

	virtual void sendMessage(const QString &message) = 0;

public slots:
	virtual void cancel() = 0;

signals:
	void gatewayAssigned(const QString &number, const QString &gatewayId);

	void progress(const QString &entryIcon, const QString &entryMessage);
	void finished(bool ok, const QString &entryIcon, const QString &entryMessage);
	void canceled();

};

#endif // SMS_SENDER_H
