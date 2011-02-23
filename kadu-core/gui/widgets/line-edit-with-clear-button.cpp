/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Piotr Pełzowski (floss@pelzowski.eu)
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

#include <QtGui/QMouseEvent>
#include <QtGui/QStyle>

#include "gui/widgets/line-edit-clear-button.h"
#include "icons-manager.h"

#include "line-edit-with-clear-button.h"

LineEditWithClearButton::LineEditWithClearButton(QWidget *parent) :
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

LineEditWithClearButton::~LineEditWithClearButton()
{
}

void LineEditWithClearButton::updateClearButton()
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

void LineEditWithClearButton::resizeEvent(QResizeEvent *e)
{
	QLineEdit::resizeEvent(e);
	updateClearButton();
}

void LineEditWithClearButton::updateClearButtonIcon(const QString& text)
{
	if (!ClearFilterButton || isReadOnly())
		return;

	bool visible = WideEnoughForClear && text.length() > 0;
	ClearFilterButton->animateVisible(visible);

	if (!ClearFilterButton->pixmap().isNull())
		return;

	if (layoutDirection() == Qt::LeftToRight)
		ClearFilterButton->setPixmap(IconsManager::instance()->iconByPath("edit-clear-locationbar-rtl").pixmap(16, 16));
	else
		ClearFilterButton->setPixmap(IconsManager::instance()->iconByPath("edit-clear-locationbar-ltr").pixmap(16, 16));

	ClearFilterButton->setVisible(text.length());
}

void LineEditWithClearButton::mousePressEvent(QMouseEvent *e)
{
	if ((e->button() == Qt::LeftButton || e->button() == Qt::MidButton) && ClearFilterButton)
		ClickInClear = ClearFilterButton->underMouse();
	QLineEdit::mousePressEvent(e);
}

void LineEditWithClearButton::mouseReleaseEvent(QMouseEvent *e)
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
