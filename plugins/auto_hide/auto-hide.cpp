/*
 * %kadu copyright begin%
 * Copyright 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "auto-hide.h"

#include "plugins/idle/idle-plugin-object.h"
#include "plugins/idle/idle.h"

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/application.h"
#include "core/core.h"
#include "gui/windows/kadu-window.h"

AutoHide::AutoHide(QObject *parent) :
		QObject{parent},
		m_idleTime{0},
		m_enabled{false}
{
	connect(&m_timer, SIGNAL(timeout()), this, SLOT(timerTimeoutSlot()));
	configurationUpdated();
}

AutoHide::~AutoHide()
{
	m_timer.stop();
}

void AutoHide::setIdle(Idle *idle)
{
	m_idle = idle;
}

void AutoHide::timerTimeoutSlot()
{
	if (!m_enabled)
		return;

	if (m_idle->secondsIdle() >= m_idleTime)
	{
		auto window = Core::instance()->kaduWindow();
		if (window->docked())
			window->window()->hide();
	}
}

void AutoHide::configurationUpdated()
{
	m_idleTime = Application::instance()->configuration()->deprecatedApi()->readNumEntry("PowerKadu", "auto_hide_idle_time", 5 * 60);
	m_enabled = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("PowerKadu", "auto_hide_use_auto_hide");

	if (m_enabled && !m_timer.isActive())
		m_timer.start(1000);
	else if (!m_enabled && m_timer.isActive())
		m_timer.stop();
}

#include "moc_auto-hide.cpp"
