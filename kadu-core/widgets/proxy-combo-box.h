/*
 * %kadu copyright begin%
 * Copyright 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "misc/memory.h"
#include "network/proxy/network-proxy.h"
#include "widgets/actions-combo-box.h"
#include "exports.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class InjectedFactory;
class NetworkProxyModel;
class ProxyEditWindowService;

class KADUAPI ProxyComboBox : public ActionsComboBox
{
	Q_OBJECT

public:
	explicit ProxyComboBox(QWidget *parent = nullptr);
	virtual ~ProxyComboBox();

	void enableDefaultProxyAction();
	void selectDefaultProxy();
	bool isDefaultProxySelected();

	void setCurrentProxy(const NetworkProxy &networkProxy);
	NetworkProxy currentProxy();

private:
	QPointer<InjectedFactory> m_injectedFactory;
	QPointer<ProxyEditWindowService> m_proxyEditWindowService;

	owned_qptr<NetworkProxyModel> m_model;
	owned_qptr<QAction> m_editProxyAction;
	owned_qptr<QAction> m_defaultProxyAction;

private slots:
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
	INJEQT_SET void setProxyEditWindowService(ProxyEditWindowService *proxyEditWindowService);
	INJEQT_INIT void init();

	void editProxy();

};
