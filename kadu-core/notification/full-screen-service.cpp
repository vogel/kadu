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

#include "full-screen-service.h"

#if defined(Q_OS_UNIX)
#include "notification/x11-screen-mode-checker.h"
#elif defined(Q_OS_WIN)
#include "notification/windows-screen-mode-checker.h"
#else
#include "notification/screen-mode-checker.h"
#endif

FullScreenService::FullScreenService(QObject *parent) :
		QObject{parent},
		m_hasFullscreenApplication{false},
		m_started{0}
{
}

FullScreenService::~FullScreenService()
{
}

bool FullScreenService::hasFullscreenApplication() const
{
	return m_hasFullscreenApplication;
}

void FullScreenService::start()
{
	m_started++;
	
	if (m_fullscreenChecker)
		return;

#if defined(Q_OS_UNIX)
	m_fullscreenChecker = not_owned_qptr<ScreenModeChecker>(new X11ScreenModeChecker{});
#elif defined(Q_OS_WIN)
	m_fullscreenChecker = not_owned_qptr<ScreenModeChecker>(new WindowsScreenModeChecker{});
#else
	m_fullscreenChecker = not_owned_qptr<ScreenModeChecker>(new ScreenModeChecker{});
#endif
	connect(m_fullscreenChecker, &ScreenModeChecker::fullscreenToggled, this, &FullScreenService::fullscreenToggledSlot);
}

void FullScreenService::stop()
{
	m_started--;

	if (m_started == 0)
	{
		m_fullscreenChecker.reset();
		fullscreenToggledSlot(false);
	}
}

void FullScreenService::fullscreenToggledSlot(bool fullscreen)
{
	if (m_hasFullscreenApplication == fullscreen)
		return;

	m_hasFullscreenApplication = fullscreen;
	emit fullscreenToggled(fullscreen);
}
