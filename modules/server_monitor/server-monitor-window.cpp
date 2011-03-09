/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010 Ziemniak (jziemkiewicz@gmail.com)
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
 *gc
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>


#include "core/core.h"
#include "configuration/configuration-file.h"
#include "gui/actions/action-description.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/main-configuration-window.h"
#include "protocols/protocols-manager.h"
#include "protocols/protocol-factory.h"
#include "misc/misc.h"
#include "modules/gadu_protocol/server/gadu-servers-manager.h"
#include "debug.h"

#include "server-monitor-window.h"

ServerMonitorWindow::ServerMonitorWindow(QWidget *parent) :
		QScrollArea(parent), AvailableServers(0), UnavailableServers(0),
		UnknownStatusServers(0), Layout(0), ScrollBarLayout(0)
{
	ButtonRefresh = new QPushButton(tr("Refresh"), this);
	ButtonRefresh->setGeometry(420, 75, 60, 25);

	connect(ButtonRefresh, SIGNAL (clicked(bool)), this, SLOT (refreshList()));
	connect(&RefreshTimer, SIGNAL (timeout()),  this, SLOT (refreshList()));
	connect(&RefreshTimer, SIGNAL (timeout()), &RefreshTimer, SLOT (start()));

	StatsLabel = new QLabel(tr("No information available"), this);
	StatsLabel->setGeometry(420, 20, 150, 50);

	configurationUpdated();

	setFixedWidth(600);

	setWindowTitle(tr("Server monitor"));
}

ServerMonitorWindow::~ServerMonitorWindow()
{
}

void ServerMonitorWindow::updateStats(ServerStatusWidget::ServerState newStatus, ServerStatusWidget::ServerState oldStatus)
{
	switch (newStatus)
	{
		case ServerStatusWidget::Available:
			AvailableServers++;
		break;

		case ServerStatusWidget::Unavailable:
			UnavailableServers++;
		break;

		case ServerStatusWidget::Unknown:
			UnknownStatusServers++;
		break;

		case ServerStatusWidget::Empty:
		break;
	}

	switch (oldStatus)
	{
		case ServerStatusWidget::Available:
			AvailableServers--;
		break;

		case ServerStatusWidget::Unavailable:
			UnavailableServers--;
		break;

		case ServerStatusWidget::Unknown:
			UnknownStatusServers--;
		break;

		case ServerStatusWidget::Empty:
		break;
	}
	StatsLabel->setText(tr("Available\t%1\nUnavailable\t%2").arg(QString::number(AvailableServers)).arg(QString::number(UnavailableServers)));
}

void ServerMonitorWindow::loadServers()
{
	ScrollBarLayout = new QWidget(this);
	Layout = new QGridLayout(ScrollBarLayout);

	AvailableServers = 0;
	UnavailableServers = 0;
	ServerStatusWidgetList.clear();

	(ProtocolsManager::instance()->byName("gadu") && config_file.readBoolEntry("serverMonitor", "useGaduServersList", true))?
			loadServersListFromGaduManager() : loadServersListFromFile();

	int serverCounter = 0;
	foreach (ServerStatusWidget* serverStatusWidget, ServerStatusWidgetList)
	{
		int row = serverCounter;
		if (serverCounter % 2)
			--row;
		Layout->addWidget(serverStatusWidget, row, (serverCounter) % 2);

		connect (serverStatusWidget, SIGNAL(statusChanged (ServerStatusWidget::ServerState, ServerStatusWidget::ServerState)),
			this, SLOT(updateStats (ServerStatusWidget::ServerState, ServerStatusWidget::ServerState)));

		serverCounter++;
	}

	ScrollBarLayout->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	ScrollBarLayout->setFixedWidth(500);
	setWidget(ScrollBarLayout);
}

void ServerMonitorWindow::loadServersListFromGaduManager()
{
	foreach (const GaduServersManager::GaduServer &server, GaduServersManager::instance()->getServersList())
		if (server.first.toIPv4Address() != (quint32)0)
			ServerStatusWidgetList.push_back(new ServerStatusWidget(server.first.toString(), server.second, "", this));
}

void ServerMonitorWindow::loadServersListFromFile()
{
	if (!QFile::exists(ServerFileListName))
		return;

	QFile serverFileList(ServerFileListName);

	serverFileList.open( QIODevice::ReadOnly);

	if (!serverFileList.isOpen())
	{
		QLabel *labelInfo = new QLabel(tr("Cannot read server list!"));
		Layout->addWidget(labelInfo, 1, 1);
		StatsLabel->setText(tr("No information available"));
	}

	while (!serverFileList.atEnd())
	{
		QString line = serverFileList.readLine();
		QStringList lineSpilted = line.split(':');

		QString addr = lineSpilted[0];
		QString port;
		QString name;

		if (lineSpilted.length() > 1)
		{
			port = lineSpilted[1];

			if (lineSpilted.length() > 2)
				name = lineSpilted[2];
		}
		ServerStatusWidgetList.push_back(new ServerStatusWidget(addr, port.toInt(), name, this));
	}
	serverFileList.close();
}

void ServerMonitorWindow::configurationUpdated()
{
	config_file.readBoolEntry("serverMonitor", "showResetButton", false) ? ButtonRefresh->show() : ButtonRefresh->hide();

	config_file.readBoolEntry("serverMonitor", "autorefresh", true) ?
			RefreshTimer.start(60000 * config_file.readNumEntry("serverMonitor", "timerInterval", 5)) : RefreshTimer.stop();

	ServerFileListName = config_file.readEntry("serverMonitor", "fileName", dataPath("kadu/modules/configuration/serverslist.txt"));
	loadServers();

	kdebugf2();
}

void ServerMonitorWindow::refreshList()
{
	foreach (ServerStatusWidget* server, ServerStatusWidgetList)
		server->refreshIcon();
}
