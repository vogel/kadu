/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Ziemniak (jziemkiewicz@gmail.com)
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

#ifndef SERVER_MONITOR_H
#define SERVER_MONITOR_H

#include <QHostAddress>

#include "gui/windows/main-configuration-window.h"

class ActionDescription;
class NotifyEvent;
class ServerMonitorWindow;

class ServerMonitor : public ConfigurationUiHandler
{
	Q_OBJECT

	ActionDescription *ServerMonitorActionDescription;
	ServerMonitorWindow *Dialog;

private slots:
	void serverMonitorActionActivated(QAction * ,bool);

public:
	explicit ServerMonitor(QWidget *parent = 0);
	virtual ~ServerMonitor();

	static NotifyEvent *notifyEvent;

public slots:
	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

};

extern ServerMonitor *serverMonitor;

#endif // SERVER_MONITOR_H
