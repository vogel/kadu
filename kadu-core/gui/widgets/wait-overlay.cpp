/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QTimer>
#include <QtGui/QLabel>
#include <QtGui/QMovie>
#include <QtGui/QResizeEvent>

#include "icons/kadu-icon.h"

#include "wait-overlay.h"

WaitOverlay::WaitOverlay(QWidget *parent) :
		QLabel(parent)
{
	setAlignment(Qt::AlignCenter);
	setMovie(new QMovie(KaduIcon("kadu_icons/please-wait", "64x64").fullPath(), QByteArray(), this));
	setStyleSheet("background-color: rgba(127, 127, 127, 127)");

	hide();

	if (parent)
		QTimer::singleShot(500, this, SLOT(timeoutPassed()));
}

WaitOverlay::~WaitOverlay()
{
}

void WaitOverlay::timeoutPassed()
{
	if (!parentWidget()) // in case of reparenting
		return;

	movie()->start();

	move(0, 0);
	resize(parentWidget()->size());
	parentWidget()->installEventFilter(this);

	show();
	raise();
}

bool WaitOverlay::eventFilter(QObject *object, QEvent *event)
{
	if (QEvent::Resize != event->type())
		return QWidget::eventFilter(object, event);

	QResizeEvent *resizeEvent = static_cast<QResizeEvent *>(event);
	resize(resizeEvent->size());

	return QWidget::eventFilter(object, event);
}
