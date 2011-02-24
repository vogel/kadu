/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Ziemniak (jziemkiewicz@gmail.com)
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

#ifndef SERVER_STATUS_WIDGET_H
#define SERVER_STATUS_WIDGET_H

#include <QtGui/QWidget>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QHostAddress>

class QLabel;

class ServerStatusWidget : public QWidget
{
	Q_OBJECT

public:
	enum ServerState
	{
		Available,
		Unavailable,
		Unknown,
		Empty
	};

private:
	QLabel *PixmapLabel;

	QHostAddress WatchedAddress;
	quint16 WatchedPort;

	ServerState CurrentState;
	QAbstractSocket::SocketState PreviousSocketState;
	QTcpSocket TcpSocket;
	QString WatchedHostDisplayName;

	void setNewState(ServerState newState);
	void notify(const QString &address, ServerState newServerState);

private slots:
	void connected();
	void connectionError(QAbstractSocket::SocketError socketError);

public:
	explicit ServerStatusWidget(const QString &watchedAddress, quint16 watchedPort = 8074, const QString &hostName = QString(), QWidget *parent = 0);
	virtual ~ServerStatusWidget();

	QString serverStateToString(ServerState serverState);

public slots:
	void refreshIcon();

signals:
	void statusChanged(ServerStatusWidget::ServerState, ServerStatusWidget::ServerState);
	void statusChanged(const QString &, ServerStatusWidget::ServerState);

};

#endif // SERVER_STATUS_WIDGET_H
