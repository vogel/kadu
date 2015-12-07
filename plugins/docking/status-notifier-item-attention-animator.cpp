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

#include "status-notifier-item-attention-animator.h"

#include <QtGui/QMovie>
#include <QtWidgets/QSystemTrayIcon>

StatusNotifierItemAttentionAnimator::StatusNotifierItemAttentionAnimator(QString moviePath, QSystemTrayIcon *systemTrayIcon, QObject *parent) :
		StatusNotifierItemAttention{parent},
		m_systemTrayIcon{systemTrayIcon}
{
	m_movie = make_owned<QMovie>(this);
	m_movie->setFileName(moviePath);
	connect(m_movie.get(), SIGNAL(updated(QRect)), this, SLOT(frameChanged()));
	m_movie->start();
}

StatusNotifierItemAttentionAnimator::~StatusNotifierItemAttentionAnimator()
{
}

void StatusNotifierItemAttentionAnimator::frameChanged()
{
	m_systemTrayIcon->setIcon(m_movie->currentPixmap());
}

#include "moc_status-notifier-item-attention-animator.cpp"
