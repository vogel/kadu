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

#include "status-notifier-item.h"

#include "status-notifier-item-attention-animator.h"
#include "status-notifier-item-attention-blinker.h"
#include "status-notifier-item-attention-mode.h"
#include "status-notifier-item-attention-static.h"

#include "icons/kadu-icon.h"

#include <QtGui/QIcon>
#include <QtGui/QMovie>
#include <QtWidgets/QMenu>

StatusNotifierItem::StatusNotifierItem(QObject *parent) :
		QObject{parent},
		m_needAttention{false}
{
	m_systemTrayIcon = make_owned<QSystemTrayIcon>(this);
	m_systemTrayIcon->setContextMenu(new QMenu{});
	m_systemTrayIcon->show();

	connect(m_systemTrayIcon.get(), SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(activated(QSystemTrayIcon::ActivationReason)));
	connect(m_systemTrayIcon.get(), SIGNAL(messageClicked()), this, SIGNAL(messageClicked()));
}

StatusNotifierItem::~StatusNotifierItem()
{
}

void StatusNotifierItem::setConfiguration(StatusNotifierItemConfiguration configuration)
{
	m_configuration = std::move(configuration);
	updateAttention();
}

void StatusNotifierItem::setIconLoader(std::function<QIcon(const QString &)> iconLoader)
{
	m_iconLoader = std::move(iconLoader);
}

void StatusNotifierItem::setNeedAttention(bool needAttention)
{
	m_needAttention = needAttention;
	updateAttention();
}

void StatusNotifierItem::updateAttention()
{
	m_attention.reset();

	if (!m_needAttention)
	{
		m_systemTrayIcon->setIcon(m_iconLoader(m_configuration.Icon));
		return;
	}

	switch (m_configuration.AttentionMode)
	{
		case StatusNotifierItemAttentionMode::StaticIcon:
			m_attention = new StatusNotifierItemAttentionStatic{m_iconLoader(m_configuration.AttentionIcon), m_systemTrayIcon.get()};
			break;
		case StatusNotifierItemAttentionMode::Movie:
			m_attention = new StatusNotifierItemAttentionAnimator{m_configuration.AttentionMovie, m_systemTrayIcon.get()};
			break;
		default:
			m_attention = new StatusNotifierItemAttentionBlinker{m_iconLoader(m_configuration.Icon), m_iconLoader(m_configuration.AttentionIcon), m_systemTrayIcon.get()};
			break;
	}
}

void StatusNotifierItem::setTooltip(const QString &tooltip)
{
#ifdef Q_OS_WIN
	// checked on XP and 7
	auto const maxTooltipLength = 127;
	auto truncatedTooltip = tooltip.length() > maxTooltipLength
			? tooltip.left(maxTooltipLength - 3) + QLatin1String{"..."}
			: tooltip;
	m_systemTrayIcon->setToolTip(truncatedTooltip);
#else
	m_systemTrayIcon->setToolTip(tooltip);
#endif
}

void StatusNotifierItem::showMessage(QString title, QString message, QSystemTrayIcon::MessageIcon icon, int msecs)
{
	m_systemTrayIcon->showMessage(std::move(title), std::move(message), icon, msecs);
}

QPoint StatusNotifierItem::trayPosition()
{
	auto rect = m_systemTrayIcon->geometry();
	if (rect.isValid())
		m_systemTrayLastPosition = rect.topLeft();

	return m_systemTrayLastPosition;
}

QMenu * StatusNotifierItem::contextMenu()
{
	return m_systemTrayIcon->contextMenu();
}

void StatusNotifierItem::activated(QSystemTrayIcon::ActivationReason reason)
{
	if (reason == QSystemTrayIcon::Trigger)
		emit activateRequested();
}

#include "moc_status-notifier-item.cpp"
