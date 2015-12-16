/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "status-notifier-item-attention-blinker.h"

#include <QtCore/QTimer>
#include <QtWidgets/QSystemTrayIcon>

StatusNotifierItemAttentionBlinker::StatusNotifierItemAttentionBlinker(QIcon normalIcon, QIcon blinkIcon, QSystemTrayIcon *systemTrayIcon, QObject *parent) :
		StatusNotifierItemAttention{parent},
		m_normalIcon{std::move(normalIcon)},
		m_blinkIcon{std::move(blinkIcon)},
		m_systemTrayIcon{systemTrayIcon},
		m_timer{make_owned<QTimer>(this)},
		m_blink{false}
{
	connect(m_timer.get(), SIGNAL(timeout()), this, SLOT(timeout()));
	m_timer->start(500);
}

StatusNotifierItemAttentionBlinker::~StatusNotifierItemAttentionBlinker()
{
	m_timer.get()->stop();
	m_systemTrayIcon->setIcon(m_normalIcon);
}

void StatusNotifierItemAttentionBlinker::timeout()
{
	m_blink = !m_blink;
	m_systemTrayIcon->setIcon(m_blink ? m_blinkIcon : m_normalIcon);
}

#include "moc_status-notifier-item-attention-blinker.cpp"
