/*
 * %kadu copyright begin%
 * Copyright 2011 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef PROXY_EDIT_WINDOW_H
#define PROXY_EDIT_WINDOW_H

#include <QtGui/QItemSelection>
#include <QtGui/QWidget>

#include "gui/widgets/modal-configuration-widget.h"
#include "network/proxy/network-proxy.h"

class QComboBox;
class QLineEdit;
class QListView;
class QPushButton;

class NetworkProxyModel;
class NetworkProxyProxyModel;

class ProxyEditWindow : public QWidget
{
	Q_OBJECT

	static ProxyEditWindow *Instance;

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

	explicit ProxyEditWindow(QWidget *parent = 0);

	void createGui();
	void updateProxyView();
	void saveProxy(NetworkProxy proxy);

	ConfigurationValueState state(NetworkProxy proxy);
	ConfigurationValueState state();
	bool canChangeProxy(const QItemSelection &selection);

	void selectProxy(NetworkProxy proxy);

private slots:
	void saveProxyButtonClicked();
	void cancelButtonClicked();
	void removeButtonClicked();

	void dataChanged();
	void proxySelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

protected:
	virtual void keyPressEvent(QKeyEvent *e);

public:
	static void show();

	virtual ~ProxyEditWindow();

};

#endif // PROXY_EDIT_WINDOW_H
