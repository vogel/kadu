/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QEvent>
#include <QtCore/QTimeLine>
#include <QtGui/QPainter>

#include "line-edit-clear-button.h"

#define ANIMATION_FRAMES_COUNT 255

LineEditClearButton::LineEditClearButton(QWidget *parent) :
		QWidget(parent)
{
	setUpTimeLine();
}

LineEditClearButton::~LineEditClearButton()
{
}

void LineEditClearButton::setUpTimeLine()
{
	  Timeline = new QTimeLine(200, this);
	  Timeline->setFrameRange(0, ANIMATION_FRAMES_COUNT);
	  Timeline->setCurveShape(QTimeLine::EaseInOutCurve);
	  Timeline->setDirection(QTimeLine::Backward);
	  connect(Timeline, SIGNAL(finished()), this, SLOT(animationFinished()));
	  connect(Timeline, SIGNAL(frameChanged(int)), this, SLOT(update()));
}

void LineEditClearButton::animateVisible(bool visible)
{
	if (visible)
	{
		if (Timeline->direction() == QTimeLine::Forward)
			return;
		Timeline->setDirection(QTimeLine::Forward);
		Timeline->setDuration(150);
	}
	else
	{
		if (Timeline->direction() == QTimeLine::Backward)
			return;
		Timeline->setDirection(QTimeLine::Backward);
		Timeline->setDuration(250);
	}

	if (QTimeLine::Running != Timeline->state())
		Timeline->start();

	if (visible)
		setCursor(Qt::ArrowCursor);
	else
		unsetCursor();

	setAttribute(Qt::WA_TransparentForMouseEvents, !visible);

	if (visible)
		setVisible(true);
}

void LineEditClearButton::setPixmap(const QPixmap &pixmap)
{
	ButtonPixmap = pixmap;
	ButtonIcon = QIcon(pixmap);
}

void LineEditClearButton::setAnimationsEnabled(bool animationsEnabled)
{
	// We need to set the current time in the case that we had the clear
	// button shown, for it being painted on the paintEvent(). Otherwise
	// it wont be painted, resulting (m->timeLine->currentTime() == 0) true,
	// and therefore a bad painting. This is needed for the case that we
	// come from a non animated widget and want it animated. (ereslibre)
	if (animationsEnabled && Timeline->direction() == QTimeLine::Forward)
	    Timeline->setCurrentTime(150);
}

void LineEditClearButton::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event)

	QPainter painter(this);
	painter.setOpacity(1.0 * Timeline->currentFrame() / ANIMATION_FRAMES_COUNT);
	painter.drawPixmap((width() - ButtonPixmap.width()) / 2,
			(height() - ButtonPixmap.height()) / 2,
			ButtonPixmap);
}

bool LineEditClearButton::event(QEvent *event)
{
	if (event->type() == QEvent::EnabledChange)
		ButtonPixmap = ButtonIcon.pixmap(ButtonPixmap.size(), isEnabled() ? QIcon::Normal : QIcon::Disabled);

	return QWidget::event(event);
}

void LineEditClearButton::animationFinished()
{
	if (Timeline->direction() == QTimeLine::Forward)
		update();
	else
		setVisible(false);
}

#include "moc_line-edit-clear-button.cpp"
