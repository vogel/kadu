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

#ifndef SERVER_MONITOR_WINDOW_H
#define SERVER_MONITOR_WINDOW_H

#include <QtCore/QTimer>
#include <QtGui/QLabel>
#include <QtGui/QScrollArea>
#include <QtGui/QPushButton>

#include "configuration/configuration-aware-object.h"

#include "server-status-widget.h"

class QBuffer;
class QGridLayout;
class QHttp;

class ServerMonitorWindow : public QScrollArea, ConfigurationAwareObject
{
	Q_OBJECT

	QList<ServerStatusWidget*> servers;
	QString serverFileListName;

	QPushButton buttonRefresh;
	QTimer refreshTimer;

	QLabel stats;
	quint32 avalibleServers;
	quint32 unavalibleServers;
	quint32 unknownStatusServers;

	QHttp   *http;
	QBuffer *serverListBuffer;

	QGridLayout *layout;
	QWidget *scrollBarLayout;

	virtual void configurationUpdated();

	void setConfiguration();
	void removeAllServer();
	void cleanLayout();

private slots:
	void downloadedServersList(bool);
	void readServerList();
	void refreshList();
	void updateStats( ServerStatusWidget::ServerState, ServerStatusWidget::ServerState );

public:
	explicit ServerMonitorWindow(QWidget *parent = 0);
	virtual ~ServerMonitorWindow();

};

#endif // SERVER_MONITOR_WINDOW_H
