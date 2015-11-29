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

#include <QtCore/QEvent>
#include <QtGui/QIcon>
#include <QtGui/QMouseEvent>
#include <QtGui/QMovie>
#include <KStatusNotifierItem>

#include "plugins/docking/docking.h"

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/core.h"
#include "icons/kadu-icon.h"
#include "debug.h"
#include "exports.h"

#include <provider/default-provider.h>

/**
 * @ingroup qdocking
 * @{
 */

StatusNotifierItem::StatusNotifierItem(QObject *parent) :
		QObject{parent},
		m_movie{nullptr}
{
	kdebugf();

	m_statusNotifierItem = new KStatusNotifierItem{this};
	m_statusNotifierItem->setAssociatedWidget(Core::instance()->mainWindowProvider()->provide());
	// see #3020: do not try to use m_statusNotifierItem->setIconByPixmap(DockingManager::instance()->defaultIcon())
	// won't work for SVG icons
	m_statusNotifierItem->setIconByPixmap(QIcon::fromTheme(DockingManager::instance()->defaultIcon().fullPath()));
	m_statusNotifierItem->setCategory(KStatusNotifierItem::Communications);
	m_statusNotifierItem->setContextMenu(DockingManager::instance()->dockMenu());
	m_statusNotifierItem->setStandardActionsEnabled(false);
	m_statusNotifierItem->setStatus(KStatusNotifierItem::Active);
	m_statusNotifierItem->setTitle("Kadu");

	

	//connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));

	//show();
	//setContextMenu(DockingManager::instance()->dockMenu());

	kdebugf2();
}

StatusNotifierItem::~StatusNotifierItem()
{
	kdebugf();

	if (m_movie)
	{
		m_movie->stop();
		m_movie->deleteLater();
		m_movie = 0;
	}

	//disconnect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));

	kdebugf2();
}

void StatusNotifierItem::changeTrayIcon(const KaduIcon &icon)
{
	if (m_movie)
	{
		m_movie->stop();
		m_movie->deleteLater();
		m_movie = 0;
	}

	// see #3020: do not try to use m_statusNotifierItem->setIconByPixmap(DockingManager::instance()->defaultIcon())
	// won't work for SVG icons
	m_statusNotifierItem->setIconByPixmap(QIcon::fromTheme(icon.fullPath()));
}

void StatusNotifierItem::changeTrayMovie(const QString &moviePath)
{
	if (m_movie)
	{
		m_movie->stop();
		m_movie->deleteLater();
	}
	//else
	//	setIcon(QIcon(QString()));

	m_movie = new QMovie(moviePath);
	m_movie->start();
	connect(m_movie, SIGNAL(updated(const QRect &)), this, SLOT(movieUpdate()));
}

void StatusNotifierItem::changeTrayTooltip(const QString &)
{
#ifdef Q_OS_WIN
	// checked on XP and 7
	const int maxTooltipLength = 127;
	const QString &truncatedTooltip = tooltip.length() > maxTooltipLength
			? tooltip.left(maxTooltipLength - 3) + QLatin1String("...")
			: tooltip;
	setToolTip(truncatedTooltip);
#else
	//setToolTip(tooltip);
#endif
}

QPoint StatusNotifierItem::trayPosition()
{
	//QRect rect = geometry();
	//if (rect.isValid())
	//	lastPosition = QPoint(rect.x(), rect.y());

	return QPoint{};
}

void StatusNotifierItem::movieUpdate()
{
	//setIcon(m_movie->currentPixmap());
}
/*
void StatusNotifierItem::trayActivated(QSystemTrayIcon::ActivationReason reason)
{
	/ * NOTE: We don't pass right button click 'cause QSystemTrayIcon
	 * takes care of it and displays context menu for us.
	 * /
	if (reason == QSystemTrayIcon::Trigger)
	{
		QMouseEvent event(QEvent::MouseButtonPress, QPoint(0,0), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
		DockingManager::instance()->trayMousePressEvent(&event);
	}
	else if (reason == QSystemTrayIcon::MiddleClick)
	{
		QMouseEvent event(QEvent::MouseButtonPress, QPoint(0,0), Qt::MidButton, Qt::MidButton, Qt::NoModifier);
		DockingManager::instance()->trayMousePressEvent(&event);
	}
}
*/
/** @} */

#include "moc_status-notifier-item.cpp"
