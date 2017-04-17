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

#include "show-about-window-action.h"
#include "show-about-window-action.moc"

#include "core/injected-factory.h"
#include "windows/about.h"
#include "windows/kadu-window-service.h"
#include "windows/kadu-window.h"

ShowAboutWindowAction::ShowAboutWindowAction(QObject *parent)
        :   // using C++ initializers breaks Qt's lupdate
          ActionDescription(parent)
{
    setIcon(KaduIcon{"kadu_icons/about-kadu"});
    setName(QStringLiteral("aboutAction"));
    setText(tr("A&bout Kadu"));
    setType(ActionDescription::TypeMainMenu);
}

ShowAboutWindowAction::~ShowAboutWindowAction()
{
}

void ShowAboutWindowAction::setInjectedFactory(InjectedFactory *injectedFactory)
{
    m_injectedFactory = injectedFactory;
}

void ShowAboutWindowAction::setKaduWindowService(KaduWindowService *kaduWindowService)
{
    m_kaduWindowService = kaduWindowService;
}

void ShowAboutWindowAction::actionTriggered(QAction *, bool)
{
    m_injectedFactory->makeInjected<About>(m_kaduWindowService->kaduWindow())->show();
}
