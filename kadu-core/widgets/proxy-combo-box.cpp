/*
 * %kadu copyright begin%
 * Copyright 2011, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "proxy-combo-box.h"

#include "core/injected-factory.h"
#include "model/model-chain.h"
#include "model/roles.h"
#include "network/proxy/model/network-proxy-model.h"
#include "network/proxy/model/network-proxy-proxy-model.h"
#include "windows/proxy-edit-window-service.h"

#include <QtWidgets/QAction>

#define DEFAULT_PROXY_INDEX 1

ProxyComboBox::ProxyComboBox(QWidget *parent) : ActionsComboBox{parent}
{
}

ProxyComboBox::~ProxyComboBox()
{
}

void ProxyComboBox::setInjectedFactory(InjectedFactory *injectedFactory)
{
    m_injectedFactory = injectedFactory;
}

void ProxyComboBox::setProxyEditWindowService(ProxyEditWindowService *proxyEditWindowService)
{
    m_proxyEditWindowService = proxyEditWindowService;
}

void ProxyComboBox::init()
{
    addBeforeAction(make_owned<QAction>(tr(" - No proxy - "), this));

    auto chain = make_owned<ModelChain>(this);
    m_model = m_injectedFactory->makeOwned<NetworkProxyModel>(chain);
    chain->setBaseModel(m_model);
    chain->addProxyModel(make_owned<NetworkProxyProxyModel>(this));
    setUpModel(NetworkProxyRole, chain);

    m_editProxyAction = make_owned<QAction>(tr("Edit proxy configuration..."), this);
    auto editProxyActionFont = m_editProxyAction->font();
    editProxyActionFont.setItalic(true);
    m_editProxyAction->setFont(editProxyActionFont);
    m_editProxyAction->setData(true);
    connect(m_editProxyAction, SIGNAL(triggered()), this, SLOT(editProxy()));

    addAfterAction(m_editProxyAction);
}

void ProxyComboBox::enableDefaultProxyAction()
{
    m_defaultProxyAction = make_owned<QAction>(tr(" - Use Default Proxy - "), this);
    m_defaultProxyAction->setFont(QFont());
    addBeforeAction(m_defaultProxyAction);
}

void ProxyComboBox::selectDefaultProxy()
{
    if (m_defaultProxyAction)
        setCurrentIndex(DEFAULT_PROXY_INDEX);
}

bool ProxyComboBox::isDefaultProxySelected()
{
    if (m_defaultProxyAction)
        return DEFAULT_PROXY_INDEX == currentIndex();
    else
        return false;
}

void ProxyComboBox::setCurrentProxy(const NetworkProxy &networkProxy)
{
    setCurrentValue(networkProxy);
}

NetworkProxy ProxyComboBox::currentProxy()
{
    return currentValue().value<NetworkProxy>();
}

void ProxyComboBox::editProxy()
{
    m_proxyEditWindowService->show();
}

#include "moc_proxy-combo-box.cpp"
