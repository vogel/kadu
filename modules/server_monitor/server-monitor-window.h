/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010 Ziemniak (jziemkiewicz@gmail.com)
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
#include <QtGui/QScrollArea>

#include "configuration/configuration-aware-object.h"

#include "server-status-widget.h"

class QBuffer;
class QGridLayout;
class QLabel;
class QPushButton;

class ServerMonitorWindow : public QScrollArea, ConfigurationAwareObject
{
	Q_OBJECT

	QList<ServerStatusWidget *> ServerStatusWidgetList;
	QString ServerFileListName;

	QPushButton *ButtonRefresh;
	QTimer RefreshTimer;

	QLabel *StatsLabel;
	quint32 AvailableServers;
	quint32 UnavailableServers;
	quint32 UnknownStatusServers;

	QGridLayout *Layout;
	QWidget *ScrollBarLayout;

	void cleanLayout();
	void loadServersListFromGaduManager();
	void loadServersListFromFile();

private slots:
	void loadServers();
	void refreshList();
	void updateStats(ServerStatusWidget::ServerState, ServerStatusWidget::ServerState);

public:
	explicit ServerMonitorWindow(QWidget *parent = 0);
	virtual ~ServerMonitorWindow();

	virtual void configurationUpdated();
};

#endif // SERVER_MONITOR_WINDOW_H
