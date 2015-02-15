/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef NETWORK_REPLY_WRAPPER_H
#define NETWORK_REPLY_WRAPPER_H

#include <QtCore/QObject>

class QNetworkReply;

class NetworkReplyWrapper : public QObject
{
	Q_OBJECT

	QNetworkReply *Reply;

public:
	explicit NetworkReplyWrapper(QNetworkReply *reply);
	virtual ~NetworkReplyWrapper();

public slots:
	bool ok();
	QString content();
	QString redirect();

signals:
	void finished();

};

#endif // NETWORK_REPLY_WRAPPER_H
