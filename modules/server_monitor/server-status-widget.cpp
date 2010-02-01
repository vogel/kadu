/*
 * %kadu copyright begin%
 * Copyright 2010 Jan Ziemkiewicz (jziemkiewicz@gmail.com)
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

#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>

#include "notify/notification-manager.h"
#include "notify/notification.h"
#include "debug.h"
#include "icons-manager.h"

#include "server-status-widget.h"

ServerStatusWidget::ServerStatusWidget(QString watchedAddress, quint16 watchedPort, QString hostName, QWidget *parent) :
		QWidget(parent), ServerStatus(Empty), ServerOldStatus(Empty),
		WatchedAddress(watchedAddress), WatchedPort(watchedPort ? watchedPort : 8074), WatchedHostName(hostName)
{
	QHBoxLayout *layout = new QHBoxLayout(this);
	PixmapLabel = new QLabel(this);
	QLabel *textLabel = new QLabel(this);

	textLabel->setText((WatchedHostName.trimmed().length() > 0) ? WatchedHostName : WatchedAddress.toString());

	connect(&TcpSocket, SIGNAL(connected()), this, SLOT(connected()));
	connect(&TcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
			this, SLOT(connectionError(QAbstractSocket::SocketError)));
	connect(this, SIGNAL(statusChanged (QString, ServerStatusWidget::ServerState)),
			this, SLOT(notify(QString, ServerStatusWidget::ServerState)));

	StatusIcon = IconsManager::instance()->loadPixmap("Invisible");
	PixmapLabel->setPixmap(StatusIcon);

	layout->addWidget(PixmapLabel, 0);
	layout->addWidget(textLabel, 100);

	refreshIcon();
}

ServerStatusWidget::~ServerStatusWidget()
{
}

void ServerStatusWidget::emitNewStatus()
{
	TcpSocket.disconnectFromHost();
	if (ServerOldStatus == ServerStatus)
		return;

	emit statusChanged(ServerStatus, ServerOldStatus);
	emit statusChanged(WatchedAddress.toString(), ServerStatus);

	PixmapLabel->setPixmap(StatusIcon);
}


void ServerStatusWidget::connected()
{
	ServerOldStatus = ServerStatus;
	ServerStatus = Available;

	StatusIcon = IconsManager::instance()->loadPixmap("Online");
	emitNewStatus();

	TcpSocket.disconnectFromHost();
}

void ServerStatusWidget::connectionError(QAbstractSocket::SocketError socketError)
{
	ServerOldStatus = ServerStatus;
	ServerStatus = Unavailable;
	StatusIcon = IconsManager::instance()->loadPixmap("Offline");

	TcpSocket.disconnectFromHost();
	emitNewStatus();
}

void ServerStatusWidget::refreshIcon()
{
	kdebugf();
	TcpSocket.connectToHost(WatchedAddress, WatchedPort, QIODevice::ReadOnly);
	kdebugf2();
}

void ServerStatusWidget::notify(QString address, ServerStatusWidget::ServerState)
{
	if (ServerOldStatus == Empty)
		return;

	Notification *notification = new Notification("serverMonitorChangeStatus",   QIcon());

	notification->setDetails(tr("Server %1 changed status to %2").arg(address).arg(serverStateToString()));
	notification->setText("Server monitor");

	NotificationManager::instance()->notify(notification);
}

QString ServerStatusWidget::serverStateToString()
{
	switch (ServerStatus)
	{
		case Available:
			return tr( "Online" );
			
		case Unavailable:
			return tr("Unavailable");

		case Unknown:
			return tr("Unknown");

		case Empty:
			return tr("Empty");
	}

	return "";
}
