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

#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>

#include "notify/notification-manager.h"
#include "notify/notification.h"
#include "debug.h"
#include "icons-manager.h"

#include "server-status-widget.h"

ServerStatusWidget::ServerStatusWidget(const QString &watchedAddress, quint16 watchedPort, const QString &hostName, QWidget *parent) :
		QWidget(parent), WatchedAddress(watchedAddress), WatchedPort(watchedPort ? watchedPort : 8074),
		CurrentState(Empty), WatchedHostDisplayName(hostName)
{
	QHBoxLayout *layout = new QHBoxLayout(this);
	PixmapLabel = new QLabel(this);
	QLabel *textLabel = new QLabel(this);

	WatchedHostDisplayName = (WatchedHostDisplayName.trimmed().length() > 0)
			? WatchedHostDisplayName
			: QString("%1:%2").arg(WatchedAddress.toString()).arg(QString::number(WatchedPort));
	textLabel->setText(WatchedHostDisplayName);

	connect(&TcpSocket, SIGNAL(connected()), this, SLOT(connected()));
	connect(&TcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
			this, SLOT(connectionError(QAbstractSocket::SocketError)));

	PixmapLabel->setPixmap(IconsManager::instance()->iconByPath("protocols/gadu-gadu/offline").pixmap(16, 16));

	layout->addWidget(PixmapLabel, 0);
	layout->addWidget(textLabel, 100);

	refreshIcon();
}

ServerStatusWidget::~ServerStatusWidget()
{
}

void ServerStatusWidget::setNewState(ServerState newState)
{
	TcpSocket.disconnectFromHost();
	if (newState == CurrentState)
		return;

	emit statusChanged(newState, CurrentState);
	if (Empty != CurrentState)
		notify(WatchedHostDisplayName, newState);

	CurrentState = newState;

	emit statusChanged(WatchedAddress.toString(), CurrentState);

	if (Available == CurrentState)
		PixmapLabel->setPixmap(IconsManager::instance()->iconByPath("protocols/gadu-gadu/online").pixmap(16, 16));
	else
		PixmapLabel->setPixmap(IconsManager::instance()->iconByPath("protocols/gadu-gadu/offline").pixmap(16, 16));
}


void ServerStatusWidget::connected()
{
	TcpSocket.disconnectFromHost();
	setNewState(Available);
}

void ServerStatusWidget::connectionError(QAbstractSocket::SocketError socketError)
{
	Q_UNUSED(socketError)

	TcpSocket.disconnectFromHost();
	setNewState(Unavailable);
}

void ServerStatusWidget::refreshIcon()
{
	kdebugf();
	TcpSocket.connectToHost(WatchedAddress, WatchedPort, QIODevice::ReadOnly);
	kdebugf2();
}

void ServerStatusWidget::notify(const QString &address, ServerState newServerState)
{
	Notification *notification = new Notification("serverMonitorChangeStatus", QString());

	notification->setDetails(tr("Server %1 changed status to %2").arg(address).arg(serverStateToString(newServerState)));
	notification->setText("Server monitor");

	NotificationManager::instance()->notify(notification);
}

QString ServerStatusWidget::serverStateToString(ServerState serverState)
{
	switch (serverState)
	{
		case Available:
			return tr("Online");

		case Unavailable:
			return tr("Unavailable");

		case Unknown:
			return tr("Unknown");

		case Empty:
			return tr("Empty");
	}

	return QString();
}
