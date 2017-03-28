/*
 * %kadu copyright begin%
 * Copyright 2016 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "status-window-service.h"

#include "activate.h"
#include "configuration/configuration.h"
#include "core/injected-factory.h"
#include "windows/status-window.h"

StatusWindowService::StatusWindowService(QObject *parent) : QObject{parent}
{
}

StatusWindowService::~StatusWindowService()
{
}

void StatusWindowService::setConfiguration(Configuration *configuration)
{
    m_configuration = configuration;
}

void StatusWindowService::setInjectedFactory(InjectedFactory *injectedFactory)
{
    m_injectedFactory = injectedFactory;
}

StatusWindow *StatusWindowService::showDialog(StatusContainer *statusContainer, QWidget *parent)
{
    if (!statusContainer)
        return 0;

    auto dialog = getDialog(statusContainer, parent);
    dialog->show();
    _activateWindow(m_configuration, dialog);

    return dialog;
}

StatusWindow *StatusWindowService::getDialog(StatusContainer *statusContainer, QWidget *parent)
{
    if (!m_dialogs.contains(statusContainer))
    {
        auto dialog = m_injectedFactory->makeInjected<StatusWindow>(statusContainer, parent);
        connect(
            dialog, SIGNAL(statusWindowClosed(StatusContainer *)), this, SLOT(statusWindowClosed(StatusContainer *)));
        m_dialogs.insert(statusContainer, dialog);
        return dialog;
    }
    else
        return m_dialogs.value(statusContainer);
}

void StatusWindowService::statusWindowClosed(StatusContainer *statusContainer)
{
    m_dialogs.remove(statusContainer);
}

#include "moc_status-window-service.cpp"
