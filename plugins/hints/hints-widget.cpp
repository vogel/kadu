/*
 * %kadu copyright begin%
 * Copyright 2016 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "hints-widget.h"

#include "hint.h"

#include "misc/memory.h"

#include <QtWidgets/QVBoxLayout>

HintsWidget::HintsWidget(QWidget *parent) :
		QFrame{parent, Qt::FramelessWindowHint | Qt::Tool | Qt::X11BypassWindowManagerHint | Qt::WindowStaysOnTopHint |Qt::MSWindowsOwnDC}
{
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	m_layout = make_owned<QVBoxLayout>(this);
	m_layout->setSpacing(0);
	m_layout->setMargin(0);

}

HintsWidget::~HintsWidget()
{
}

void HintsWidget::addHint(Hint *hint)
{
	m_layout->addWidget(hint);
}

void HintsWidget::removeHint(Hint *hint)
{
	m_layout->removeWidget(hint);
}

void HintsWidget::setDirection(Direction direction)
{
	switch (direction)
	{
		case Direction::Down:
			m_layout->setDirection(QBoxLayout::Down);
			break;
		case Direction::Up:
			m_layout->setDirection(QBoxLayout::Up);
			break;
	}
}

void HintsWidget::resizeEvent(QResizeEvent *re)
{
	QWidget::resizeEvent(re);
	emit sizeChanged();
}

void HintsWidget::showEvent(QShowEvent *se)
{
	QWidget::showEvent(se);
	emit shown();
}

#include "moc_hints-widget.cpp"
