/*
 * %kadu copyright begin%
 * Copyright 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtGui/QColorDialog>

#include "color-button.h"

ColorButton::ColorButton(QWidget *parent)
	: QPushButton(parent)
{
	connect(this, SIGNAL(clicked()), this, SLOT(onClick()));
}

void ColorButton::onClick()
{
	if (setColor(QColorDialog::getColor(currentColor, this)))
		emit changed(currentColor);
}

const QColor & ColorButton::color() const
{
	return currentColor;
}

bool ColorButton::setColor(const QColor &color)
{
	if (!color.isValid())
		return false;

	currentColor = color;
	QPixmap pm(40, 20);
	pm.fill(color);
	setIcon(pm);

	return true;
}

#include "moc_color-button.cpp"
