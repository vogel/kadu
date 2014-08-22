/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Piotr Pełzowski (floss@pelzowski.eu)
 * Copyright 2012 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include "icons/kadu-icon.h"

#include "line-edit-with-clear-button.h"

LineEditWithClearButton::LineEditWithClearButton(QWidget *parent) :
		QLineEdit(parent), ClearButtonVisible(true)
{
	WideEnoughForClear = true;
	ClickInClear = false;

	ClearButton = 0;
}

LineEditWithClearButton::~LineEditWithClearButton()
{
}

void LineEditWithClearButton::createClearButton()
{
	if (ClearButton)
		return;

	ClearButton = new LineEditClearButton(this);
	ClearButton->setVisible(false);
	ClearButton->setToolTip(tr("Clear this field"));

	connect(this, SIGNAL(textChanged(const QString &)), this, SLOT(updateClearButtonIcon()));

	updateClearButtonIcon();
}

void LineEditWithClearButton::updateClearButton()
{
	WideEnoughForClear = true;
	bool visible = canShowClearButton();

	if (!ClearButton && visible)
		createClearButton();

	if (!ClearButton)
		return;

	if (layoutDirection() == Qt::LeftToRight)
		ClearButton->setPixmap(KaduIcon("edit-clear-locationbar-rtl").icon().pixmap(16, 16));
	else
		ClearButton->setPixmap(KaduIcon("edit-clear-locationbar-ltr").icon().pixmap(16, 16));

	const QSize geom = size();
	const int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth, 0, this);
	const int buttonWidth = ClearButton->sizeHint().width();
	const QSize newButtonSize(buttonWidth, geom.height());
	const QFontMetrics fm(font());
	const int em = fm.width("m");

	// make sure we have enough room for the clear button
	// no point in showing it if we can't also see a few characters
	WideEnoughForClear = geom.width() > 4 * em + buttonWidth + frameWidth;

	if (newButtonSize != ClearButton->size())
		ClearButton->resize(newButtonSize);

	if (layoutDirection() == Qt::LeftToRight)
		ClearButton->move(geom.width() - frameWidth - buttonWidth - 1, 0);
	else
		ClearButton->move(frameWidth + 1, 0);

	updateClearButtonIcon();
}

void LineEditWithClearButton::resizeEvent(QResizeEvent *e)
{
	QLineEdit::resizeEvent(e);
	updateClearButton();
}

void LineEditWithClearButton::updateClearButtonIcon()
{
	if (!ClearButton)
		return;

	bool visible = canShowClearButton() && text().length() > 0;

	ClearButton->animateVisible(visible);
}

bool LineEditWithClearButton::canShowClearButton()
{
	return ClearButtonVisible && WideEnoughForClear && !isReadOnly();
}

void LineEditWithClearButton::mousePressEvent(QMouseEvent *e)
{
	if ((e->button() == Qt::LeftButton || e->button() == Qt::MidButton) && ClearButton)
		ClickInClear = ClearButton->underMouse();
	QLineEdit::mousePressEvent(e);
}

void LineEditWithClearButton::mouseReleaseEvent(QMouseEvent *e)
{
	if (ClickInClear)
	{
		if (ClearButton->underMouse())
		{
			clear();
			emit cleared();
		}

		ClickInClear = false;
		e->accept();
		return;
	}

	QLineEdit::mouseReleaseEvent(e);
}

void LineEditWithClearButton::setClearButtonVisible(bool clearButtonVisible)
{
	if (clearButtonVisible == ClearButtonVisible)
		return;

	ClearButtonVisible = clearButtonVisible;
	updateClearButton();
}

void LineEditWithClearButton::setReadOnly(bool readonly)
{
	if (isReadOnly() == readonly)
		return;

	QLineEdit::setReadOnly(readonly);
	updateClearButton();
}

void LineEditWithClearButton::setEnabled(bool enabled)
{
	if (isEnabled() == enabled)
		return;

	QLineEdit::setEnabled(enabled);
	updateClearButton();
}

#include "moc_line-edit-with-clear-button.cpp"
