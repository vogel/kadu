/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "gui/actions/action-description.h"

#include <QtCore/QPointer>
#include <QtWidgets/QAction>
#include <injeqt/injeqt.h>

class Actions;
class NetworkProxyManager;
class NetworkProxy;
class ProxyEditWindowService;

class DefaultProxyAction : public ActionDescription
{
	Q_OBJECT

public:
	explicit DefaultProxyAction(QObject *parent = nullptr);
	virtual ~DefaultProxyAction();

	virtual QToolButton::ToolButtonPopupMode buttonPopupMode() const
	{
		return QToolButton::InstantPopup;
	}

protected:
	virtual QMenu * menuForAction(Action *action);

private:
	QPointer<NetworkProxyManager> m_networkProxyManager;
	QPointer<ProxyEditWindowService> m_proxyEditWindowService;

	void populateMenu(QMenu *menu, QActionGroup *actionGroup, NetworkProxy defaultProxy);

private slots:
	INJEQT_SET void setNetworkProxyManager(NetworkProxyManager *networkProxyManager);
	INJEQT_SET void setProxyEditWindowService(ProxyEditWindowService *proxyEditWindowService);

	void prepareMenu();
	void selectProxyActionTriggered(QAction *action);
	void editProxyConfiguration();

};
