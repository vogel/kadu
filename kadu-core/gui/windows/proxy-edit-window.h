/*
 * %kadu copyright begin%
 * Copyright 2011 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QItemSelection>
#include <QtWidgets/QWidget>
#include <injeqt/injeqt.h>

#include "gui/widgets/modal-configuration-widget.h"
#include "network/proxy/network-proxy.h"

class QComboBox;
class QLineEdit;
class QListView;
class QPushButton;

class Configuration;
class InjectedFactory;
class NetworkProxyManager;
class NetworkProxyModel;
class NetworkProxyProxyModel;

class ProxyEditWindow : public QWidget
{
	Q_OBJECT

public:
	explicit ProxyEditWindow(QWidget *parent = nullptr);
	virtual ~ProxyEditWindow();

protected:
	virtual void keyPressEvent(QKeyEvent *e);

private:
	QPointer<Configuration> m_configuration;
	QPointer<InjectedFactory> m_injectedFactory;
	QPointer<NetworkProxyManager> m_networkProxyManager;

	QListView *ProxyView;
	NetworkProxyModel *ProxyModel;
	NetworkProxyProxyModel *ProxyProxyModel;
	QAction *AddProxyAction;

	QComboBox *Type;
	QLineEdit *Host;
	QLineEdit *Port;
	QLineEdit *User;
	QLineEdit *Password;
	QLineEdit *PollingUrl;

	QPushButton *SaveButton;
	QPushButton *CancelButton;
	QPushButton *RemoveButton;

	/*
	 * This is necessary to prevent infinite recursion when checking
	 * if we can safely change widget. Blocking signals doesn't work
	 * as expected, because ProtocolsComboBox really wants to have
	 * signals enabled to update its CurrentValue properly.
	 */
	bool ForceProxyChange;

	void createGui();
	void updateProxyView();
	void saveProxy(NetworkProxy proxy);

	ConfigurationValueState state(NetworkProxy proxy);
	ConfigurationValueState state();
	bool canChangeProxy(const QItemSelection &selection);

	void selectProxy(NetworkProxy proxy);

private slots:
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
	INJEQT_SET void setNetworkProxyManager(NetworkProxyManager *networkProxyManager);
	INJEQT_INIT void init();

	void saveProxyButtonClicked();
	void cancelButtonClicked();
	void removeButtonClicked();

	void dataChanged();
	void proxySelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

};
