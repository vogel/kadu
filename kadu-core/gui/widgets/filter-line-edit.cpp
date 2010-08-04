/*
 * %kadu copyright begin%
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
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

#include <QtGui/QStyle>

#include "icons-manager.h"

#include "filter-line-edit.h"


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
	  Timeline->setFrameRange(0, 255);
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
		show();
	}
	else
	{
		if (Timeline->direction() == QTimeLine::Backward)
		    return;

		Timeline->setDirection(QTimeLine::Backward);
		Timeline->setDuration(250);
	}

	setVisible(Timeline->direction() == QTimeLine::Forward);
}

void LineEditClearButton::setPixmap(const QPixmap& p)
{
	ButtonPixmap = p;
	ButtonIcon = QIcon(p);
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

	QPainter p(this);
	p.drawPixmap((width() - ButtonPixmap.width()) / 2,
		    (height() - ButtonPixmap.height()) / 2,
		    ButtonPixmap);
}

bool LineEditClearButton::event(QEvent* event)
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
		hide();
}

FilterLineEdit::FilterLineEdit(QWidget *parent) :
		QLineEdit(parent)
{
	WideEnoughForClear = true;
	Overlap = 0;
	ClickInClear = false;

	ClearFilterButton = new LineEditClearButton(this);
	ClearFilterButton->setCursor(Qt::ArrowCursor);
	ClearFilterButton->setToolTip(tr("Clear current text in the line edit."));
	updateClearButtonIcon(text());
	connect(this, SIGNAL(textChanged(const QString &)),
			this, SLOT(updateClearButtonIcon(const QString &)));
}

FilterLineEdit::~FilterLineEdit()
{
}

void FilterLineEdit::updateClearButton()
{
	if (!ClearFilterButton || isReadOnly())
		return;
	
	const QSize geom = size();
	const int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth, 0, this);
	const int buttonWidth = ClearFilterButton->sizeHint().width();
	const QSize newButtonSize(buttonWidth, geom.height());
	const QFontMetrics fm(font());
	const int em = fm.width("m");

	// make sure we have enough room for the clear button
	// no point in showing it if we can't also see a few characters as well
	const bool wideEnough = geom.width() > 4 * em + buttonWidth + frameWidth;
	
	if (newButtonSize != ClearFilterButton->size())
	{
		ClearFilterButton->resize(newButtonSize);
		Overlap = wideEnough ? buttonWidth + frameWidth : 0;
	}

	if (layoutDirection() == Qt::LeftToRight)
		ClearFilterButton->move(geom.width() - frameWidth - buttonWidth - 1, 0);
	else
		ClearFilterButton->move(frameWidth + 1, 0);

	if (wideEnough != WideEnoughForClear)
	{
		// we may (or may not) have been showing the button, but now our
		// positiong on that matter has shifted, so let's ensure that it
		// is properly visible (or not)
		WideEnoughForClear = wideEnough;
		updateClearButtonIcon(text());
	}
}

void FilterLineEdit::resizeEvent(QResizeEvent *e)
{
	QLineEdit::resizeEvent(e);
	updateClearButton();
}

void FilterLineEdit::updateClearButtonIcon(const QString& text)
{
	if (!ClearFilterButton || isReadOnly())
		return;

	bool visible = WideEnoughForClear && text.length() > 0;
	ClearFilterButton->animateVisible(visible);

	if (!ClearFilterButton->pixmap().isNull())
		return;

	if (layoutDirection() == Qt::LeftToRight)
		ClearFilterButton->setPixmap(IconsManager::instance()->pixmapByPath("16x16/edit-clear-rtl.png"));
	else
		ClearFilterButton->setPixmap(IconsManager::instance()->pixmapByPath("16x16/edit-clear-ltr.png"));

	ClearFilterButton->setVisible(text.length());
}

void FilterLineEdit::mousePressEvent(QMouseEvent *e)
{
	if ((e->button() == Qt::LeftButton || e->button() == Qt::MidButton) && ClearFilterButton)
		ClickInClear = ClearFilterButton->underMouse();
	QLineEdit::mousePressEvent(e);
}

void FilterLineEdit::mouseReleaseEvent(QMouseEvent *e)
{
	if (ClickInClear)
	{
		if (ClearFilterButton->underMouse())
			clear();

		ClickInClear = false;
		e->accept();
		return;
	}
  
	QLineEdit::mouseReleaseEvent(e);
}
