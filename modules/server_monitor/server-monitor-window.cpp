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

#include <QtCore/QBuffer>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtNetwork/QHttp>


#include "core/core.h"
#include "configuration/configuration-file.h"
#include "debug.h"
#include "gui/actions/action-description.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/main-configuration-window.h"
#include "misc/misc.h"

#include "server-monitor-window.h"

ServerMonitorWindow::ServerMonitorWindow(QWidget *parent):
		QScrollArea(parent), AvalibleServers(0), UnavalibleServers(0),
		UnknownStatusServers(0), Layout(0), ScrollBarLayout(0)
{
	ButtonRefresh = new QPushButton(tr("Refresh"), this);
	ButtonRefresh->setGeometry(420, 75, 60, 25);

	connect (ButtonRefresh, SIGNAL (clicked(bool) ), this, SLOT (refreshList()));
	connect (&RefreshTimer, SIGNAL (timeout()),  this, SLOT (refreshList()));
	connect (&RefreshTimer, SIGNAL (timeout()), &RefreshTimer, SLOT (start()));

	setConfiguration();

	StatsLabel = new QLabel(tr("No information avalible"), this);
	StatsLabel->setGeometry(420, 20, 150, 50);

	setFixedWidth(600);

	setWindowTitle(tr("Server monitor"));
}

ServerMonitorWindow::~ServerMonitorWindow()
{
	removeAllServer();
}

void ServerMonitorWindow::updateStats(ServerStatusWidget::ServerState newStatus, ServerStatusWidget::ServerState oldStatus)
{
	switch (newStatus)
	{
		case ServerStatusWidget::Available:
			AvalibleServers++;
		break;

		case ServerStatusWidget::Unavailable:
			UnavalibleServers++;
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
			AvalibleServers--;
		break;

		case ServerStatusWidget::Unavailable:
			UnavalibleServers--;
		break;

		case ServerStatusWidget::Unknown:
			UnknownStatusServers--;
		break;

		case ServerStatusWidget::Empty:
		break;
	}
	StatsLabel->setText(tr("Avalible\t%1\nUnavailable\t%2").arg(QString::number(AvalibleServers)).arg(QString::number(UnavalibleServers)));
}

void ServerMonitorWindow::readServerList()
{
	if (Layout == 0) delete Layout;
	if (ScrollBarLayout == 0) delete ScrollBarLayout;

	Layout = new QGridLayout(this);
	ScrollBarLayout = new QWidget(this);

	AvalibleServers = 0;
	UnavalibleServers = 0;
	removeAllServer();

	QFile serverFileList(ServerFileListName);

	serverFileList.open( QIODevice::ReadOnly);

	if (!serverFileList.isOpen())
	{
		QLabel *labelInfo = new QLabel(tr("Cannot read server list!"));
		Layout->addWidget(labelInfo, 1, 1);
		StatsLabel->setText(tr("No information avalible"));
	}

	int serverCounter = 0;
	while (!serverFileList.atEnd())
	{
		QString line = serverFileList.readLine();
		QStringList lineSpilted = line.split(':');

		QString addr = lineSpilted[0];
		QString port = "";
		QString name = "";

		if (lineSpilted.length() > 1)
		{
			port = lineSpilted[1];

			if (lineSpilted.length() > 2)
				name = lineSpilted[2];
		}
		ServerStatusWidget *serverStatusWidget = new ServerStatusWidget (addr,port.toInt(),name, this);
		ServerStatusWidgetList.push_back(serverStatusWidget);

		int row = serverCounter;
		if (serverCounter % 2)
			--row;

		Layout->addWidget(ServerStatusWidgetList[serverCounter], row, (serverCounter) % 2);

		connect (ServerStatusWidgetList[serverCounter], SIGNAL(statusChanged (ServerStatusWidget::ServerState, ServerStatusWidget::ServerState)),
			this, SLOT(updateStats (ServerStatusWidget::ServerState, ServerStatusWidget::ServerState)));

		serverCounter++;
	}

	serverFileList.close();

	ScrollBarLayout->setLayout(Layout);
	ScrollBarLayout->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	ScrollBarLayout->setFixedWidth(500);
	setWidget(ScrollBarLayout);
}


void ServerMonitorWindow::configurationUpdated()
{
	setConfiguration();
}

void ServerMonitorWindow::setConfiguration()
{
	kdebugf();

	config_file_ptr->readBoolEntry("serverMonitor", "showResetButton", false) ? ButtonRefresh->show() : ButtonRefresh->hide();

	config_file_ptr->readBoolEntry("serverMonitor", "autorefresh", true) ?
			RefreshTimer.start(60000 * config_file_ptr->readNumEntry("serverMonitor", "timerInterval", 5)) : RefreshTimer.stop();

	if (config_file_ptr->readBoolEntry("serverMonitor", "useListFromServer", false))
	{
		ServerListBuffer = new QBuffer();

		QString url = config_file_ptr->readEntry("serverMonitor", "serverListHost");
		QString hostName;
		QString path;

		int index = url.indexOf("/");
		if (index > 0)
		{
			hostName = url.left(index);
			path = "/"+url.section("/",1);
		}
		else
		{
			hostName = url;
			path = "/serverslist.txt";
		}

		Http = new QHttp (hostName, 80, this);
		Http->get(path, ServerListBuffer);
		connect (Http, SIGNAL (done(bool)),
			this, SLOT(downloadedServersList(bool)));
	}
	else
	{
		ServerFileListName = config_file_ptr->readEntry("serverMonitor", "fileName", "kadu/modules/configuration/serverslist.txt");
		readServerList();
	}

	kdebugf2();
}

void ServerMonitorWindow::downloadedServersList(bool err)
{
	kdebugf();

	if (err)
	{
		kdebugm( KDEBUG_WARNING, "Cannont download server's list!" );
		ServerFileListName = config_file_ptr->readEntry( "serverMonitor", "fileName", "kadu/modules/configuration/serverslist.txt");
		return;
	}

	QFile fileList(QDir::tempPath()+"/serverslist.txt");
	fileList.open(QIODevice::WriteOnly);
	fileList.write(ServerListBuffer->buffer());
	fileList.close();

	ServerFileListName = QDir::tempPath()+"/serverslist.txt";
	readServerList();

	//    disconnect ( http, SIGNAL ( done (bool) ),
	//        this, SLOT( downloadedServersList(bool) ));
	delete Http;
	delete ServerListBuffer;

	kdebugf2();
}

void ServerMonitorWindow::refreshList()
{
	foreach (ServerStatusWidget* server, ServerStatusWidgetList ) server->refreshIcon();
}

void ServerMonitorWindow::removeAllServer()
{
	qDeleteAll(ServerStatusWidgetList);
	ServerStatusWidgetList.clear();
}
