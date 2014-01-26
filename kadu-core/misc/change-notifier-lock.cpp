/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "change-notifier-lock.h"

#include "misc/change-notifier.h"

ChangeNotifierLock::ChangeNotifierLock(ChangeNotifier &notifier, Mode mode)
		: m_notifier(notifier), m_notifierMode{mode}
{
	m_notifier.block();
}

ChangeNotifierLock::~ChangeNotifierLock()
{
	if (ModeForget == m_notifierMode)
		m_notifier.forget();
	m_notifier.unblock();
}

void ChangeNotifierLock::setMode(Mode mode)
{
	m_notifierMode = mode;
}
