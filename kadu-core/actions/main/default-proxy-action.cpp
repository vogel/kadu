/*
 * %kadu copyright begin%
 * Copyright 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "default-proxy-action.h"
#include "default-proxy-action.moc"

#include "actions/action.h"
#include "core/injected-factory.h"
#include "model/roles.h"
#include "network/proxy/model/network-proxy-model.h"
#include "network/proxy/model/network-proxy-proxy-model.h"
#include "network/proxy/network-proxy-manager.h"
#include "windows/proxy-edit-window-service.h"

#include <QtWidgets/QAction>
#include <QtWidgets/QMenu>

DefaultProxyAction::DefaultProxyAction(QObject *parent) : ActionDescription(parent)
{
    setType(ActionDescription::TypeGlobal);
    setName("defaultProxyAction");
    setText(tr("Select Default Proxy"));
}

DefaultProxyAction::~DefaultProxyAction()
{
}

void DefaultProxyAction::setNetworkProxyManager(NetworkProxyManager *networkProxyManager)
{
    m_networkProxyManager = networkProxyManager;
}

void DefaultProxyAction::setProxyEditWindowService(ProxyEditWindowService *proxyEditWindowService)
{
    m_proxyEditWindowService = proxyEditWindowService;
}

QMenu *DefaultProxyAction::menuForAction(Action *action)
{
    Q_UNUSED(action)

    // no parents for menu as it is destroyed manually by Action class
    QMenu *menu = new QMenu();
    connect(menu, SIGNAL(aboutToShow()), this, SLOT(prepareMenu()));
    connect(menu, SIGNAL(triggered(QAction *)), this, SLOT(selectProxyActionTriggered(QAction *)));
    return menu;
}

void DefaultProxyAction::populateMenu(QMenu *menu, QActionGroup *actionGroup, NetworkProxy defaultProxy)
{
    auto proxyModel = injectedFactory()->makeInjected<NetworkProxyModel>();
    auto proxyProxyModel = new NetworkProxyProxyModel();
    proxyProxyModel->setSourceModel(proxyModel);

    int proxCount = proxyProxyModel->rowCount();

    for (int i = 0; i < proxCount; i++)
    {
        NetworkProxy networkProxy =
            proxyProxyModel->data(proxyProxyModel->index(i, 0), NetworkProxyRole).value<NetworkProxy>();
        if (networkProxy)
        {
            QAction *proxyAction = menu->addAction(networkProxy.displayName());
            actionGroup->addAction(proxyAction);
            proxyAction->setData(QVariant::fromValue(networkProxy));
            proxyAction->setCheckable(true);
            if (defaultProxy == networkProxy)
                proxyAction->setChecked(true);
        }
    }

    delete proxyProxyModel;
    delete proxyModel;
}

void DefaultProxyAction::prepareMenu()
{
    QMenu *menu = qobject_cast<QMenu *>(sender());
    if (!menu)
        return;

    menu->clear();

    NetworkProxy defaultProxy = m_networkProxyManager->defaultProxy();

    QAction *proxyAction = menu->addAction(tr(" - No proxy - "));

    // this parenting allows proxyActions to be removed on menu->clear() whats prevents memory leak
    QActionGroup *proxyActions = new QActionGroup(proxyAction);
    proxyActions->addAction(proxyAction);
    proxyAction->setCheckable(true);
    if (!defaultProxy)
        proxyAction->setChecked(true);

    populateMenu(menu, proxyActions, defaultProxy);

    menu->addSeparator();
    QAction *editProxyConfigurationAction = menu->addAction(tr("Edit proxy configuration..."));
    connect(editProxyConfigurationAction, SIGNAL(triggered()), this, SLOT(editProxyConfiguration()));
}

void DefaultProxyAction::selectProxyActionTriggered(QAction *action)
{
    NetworkProxy defaultProxy = qvariant_cast<NetworkProxy>(action->data());
    m_networkProxyManager->setDefaultProxy(defaultProxy);
}

void DefaultProxyAction::editProxyConfiguration()
{
    m_proxyEditWindowService->show();
}
