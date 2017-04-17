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

#include "multilogon-window-service.h"
#include "multilogon-window-service.moc"

#include "activate.h"
#include "core/injected-factory.h"
#include "windows/multilogon-window.h"

MultilogonWindowService::MultilogonWindowService(QObject *parent) : QObject{parent}
{
}

MultilogonWindowService::~MultilogonWindowService()
{
}

void MultilogonWindowService::setConfiguration(Configuration *configuration)
{
    m_configuration = configuration;
}

void MultilogonWindowService::setInjectedFactory(InjectedFactory *injectedFactory)
{
    m_injectedFactory = injectedFactory;
}

void MultilogonWindowService::show()
{
    if (!m_multilogonWindow)
        m_multilogonWindow = m_injectedFactory->makeInjected<MultilogonWindow>();

    m_multilogonWindow->show();
    _activateWindow(m_configuration, m_multilogonWindow);
}
