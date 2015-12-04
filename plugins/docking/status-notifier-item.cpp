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

#include "status-notifier-item-attention-blinker.h"
#include "status-notifier-item-attention-mode.h"

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/core.h"
#include "icons/kadu-icon.h"
#include "exports.h"

#include <KStatusNotifierItem>
#include <QtCore/QEvent>
#include <QtGui/QIcon>
#include <QtGui/QMouseEvent>
#include <QtGui/QMovie>
#include <QtWidgets/QMenu>

/**
 * @ingroup docking
 * @{
 */

StatusNotifierItem::StatusNotifierItem(QObject *parent) :
		QObject{parent},
		m_attentionMode{StatusNotifierItemAttentionMode::BlinkingIcon},
		m_needAttention{false}
{
	m_statusNotifierItem = make_owned<KStatusNotifierItem>(this);
	m_statusNotifierItem->setCategory(KStatusNotifierItem::Communications);
	m_statusNotifierItem->setContextMenu(new QMenu{});
	m_statusNotifierItem->setStandardActionsEnabled(false);
	m_statusNotifierItem->setStatus(KStatusNotifierItem::Active);
	m_statusNotifierItem->setTitle("Kadu");

	connect(m_statusNotifierItem.get(), SIGNAL(activateRequested(bool,QPoint)), this, SIGNAL(activateRequested()));
}

StatusNotifierItem::~StatusNotifierItem()
{
}

void StatusNotifierItem::setAttentionMode(StatusNotifierItemAttentionMode attentionMode)
{
	m_attentionMode = attentionMode;
}

void StatusNotifierItem::setNeedAttention(bool needAttention)
{
	m_needAttention = needAttention;

	updateAttention();
}

void StatusNotifierItem::setAttentionIcon(const QString &attentionIconPath)
{
	m_attentionIconPath = attentionIconPath;

	updateAttention();
}

void StatusNotifierItem::setAttentionMovie(const QString &attentionMoviePath)
{
	m_attentionMoviePath = attentionMoviePath;

	updateAttention();
}

void StatusNotifierItem::setIcon(const QString &iconPath)
{
	m_iconPath = iconPath;
	m_statusNotifierItem->setIconByName(iconPath);

	updateAttention();
}

void StatusNotifierItem::updateAttention()
{
	m_statusNotifierItem->setAttentionMovieByName({});
	m_statusNotifierItem->setAttentionIconByName({});

	auto blink = false;
	switch (m_attentionMode)
	{
		case StatusNotifierItemAttentionMode::StaticIcon:
			m_statusNotifierItem->setAttentionIconByName(m_attentionIconPath);
			break;
		case StatusNotifierItemAttentionMode::Movie:
			m_statusNotifierItem->setAttentionMovieByName(m_attentionMoviePath);
			break;
		default:
			m_statusNotifierItem->setStatus(KStatusNotifierItem::Active);
			blink = m_needAttention;
			break;
	}

	if (blink)
		startBlinking();
	else
		stopBlinking();

	m_statusNotifierItem->setStatus(!blink && m_needAttention
			? KStatusNotifierItem::NeedsAttention
			: KStatusNotifierItem::Active);
}

bool StatusNotifierItem::shouldBlink()
{
	if (m_statusNotifierItem->status() != KStatusNotifierItem::NeedsAttention)
		return false;

	if (m_attentionMode != StatusNotifierItemAttentionMode::BlinkingIcon)
		return false;

	return true;
}

void StatusNotifierItem::startBlinking()
{
	if (!m_blinker.get())
		m_blinker = make_not_owned<StatusNotifierItemAttentionBlinker>(m_iconPath, m_attentionIconPath, m_statusNotifierItem.get());
}

void StatusNotifierItem::stopBlinking()
{
	m_blinker.reset();
}

void StatusNotifierItem::setTooltip(const QString &tooltip)
{
#ifdef Q_OS_WIN
	// checked on XP and 7
	auto const maxTooltipLength = 127;
	auto truncatedTooltip = tooltip.length() > maxTooltipLength
			? tooltip.left(maxTooltipLength - 3) + QLatin1String{"..."}
			: tooltip;
	m_statusNotifierItem->setToolTipSubTitle(truncatedTooltip);
#else
	m_statusNotifierItem->setToolTipSubTitle(tooltip);
#endif
}

QPoint StatusNotifierItem::trayPosition()
{
	//QRect rect = geometry();
	//if (rect.isValid())
	//	lastPosition = QPoint(rect.x(), rect.y());

	return QPoint{};
}

QMenu * StatusNotifierItem::contextMenu()
{
	return m_statusNotifierItem->contextMenu();
}

/** @} */

#include "moc_status-notifier-item.cpp"
