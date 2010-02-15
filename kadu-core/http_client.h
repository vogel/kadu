/*
 * %kadu copyright begin%
 * Copyright 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2005, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008, 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#ifndef KADU_HTTP_CLIENT_H
#define KADU_HTTP_CLIENT_H

#include <QtCore/QByteArray>
#include <QtNetwork/QTcpSocket>

#include "exports.h"

// TODO: replace with QHttp
class KADUAPI HttpClient : public QObject
{
	Q_OBJECT

	QTcpSocket Socket;
	QString Host;
	QString Agent;
	QString Referer;
	QString Path;
	QByteArray Data;
	QByteArray PostData;

	int StatusCode;
	bool HeaderParsed;
	bool FollowRedirect;

	int ContentLength;
	bool ContentLengthNotFound;

	QMap<QString, QString> Cookies;

private slots:
	void onConnected();
	void onReadyRead();
	void onConnectionClosed(QAbstractSocket::SocketError errorCode);

public:
	HttpClient();
	int status() const;
	const QByteArray & data() const;

	const QString cookie(const QString &name) const;
	const QMap<QString, QString> & cookies() const;
	void setCookie(const QString & name, const QString &value);

public slots:
	void setHost(const QString &host);
	void setAgent(const QString &agent);
	void get(const QString &path, bool redirectFollow = true);
	void post(const QString &path, const QByteArray &data, bool redirectFollow = true);
	void post(const QString &path, const QString &data, bool redirectFollow = true);

signals:
	void finished();
	void redirected(QString link);
	void error();

};

#endif // KADU_HTTP_CLIENT_H
