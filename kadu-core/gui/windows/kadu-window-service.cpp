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

#include "kadu-window-service.h"

#include "core/injected-factory.h"
#include "gui/windows/kadu-window.h"

KaduWindowService::KaduWindowService(QObject *parent) :
		QObject{parent},
		m_kaduWindow{nullptr},
		m_kaduWindowProvider{new SimpleProvider<QWidget *>(nullptr)},
		m_mainWindowProvider{new DefaultProvider<QWidget *>(m_kaduWindowProvider)},
		m_showMainWindowOnStart{true}
{
}

KaduWindowService::~KaduWindowService()
{
}

void KaduWindowService::setInjectedFactory(InjectedFactory *injectedFactory)
{
	m_injectedFactory = injectedFactory;
}

void KaduWindowService::done()
{
	if (m_kaduWindow)
	{
		// TODO: check if needed and why
		m_kaduWindowProvider->provideValue(nullptr);
		auto hiddenParent = m_kaduWindow->parentWidget();
		delete m_kaduWindow;
		delete hiddenParent;
	}
}

void KaduWindowService::createWindow()
{
	m_kaduWindow = m_injectedFactory->makeInjected<KaduWindow>();
	connect(m_kaduWindow, SIGNAL(destroyed()), this, SLOT(kaduWindowDestroyed()));
	m_kaduWindowProvider->provideValue(m_kaduWindow);
}

void KaduWindowService::kaduWindowDestroyed()
{
	m_kaduWindowProvider->provideValue(nullptr);
	m_kaduWindow = 0;
}

KaduWindow * KaduWindowService::kaduWindow()
{
	return m_kaduWindow;
}

void KaduWindowService::showMainWindow()
{
	if (m_showMainWindowOnStart)
		m_mainWindowProvider->provide()->show();

	// after first call which has to be placed in main(), this method should always show main window
	m_showMainWindowOnStart = true;
}

void KaduWindowService::setShowMainWindowOnStart(bool showMainWindowOnStart)
{
	m_showMainWindowOnStart = showMainWindowOnStart;
}

std::shared_ptr<DefaultProvider<QWidget *>> KaduWindowService::mainWindowProvider() const
{
	return m_mainWindowProvider;
}

#include "moc_kadu-window-service.cpp"
