/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qcolordialog.h>
//Added by qt3to4:
#include <QPixmap>

#include "color_button.h"

ColorButton::ColorButton(QWidget *parent, const char *name)
	: QPushButton(parent, name)
{
	connect(this, SIGNAL(clicked()), this, SLOT(onClick()));
}

void ColorButton::onClick()
{
	if (setColor(QColorDialog::getColor(currentColor, this, "Color dialog")))
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
	QPixmap pm(35, 10);
	pm.fill(color);
	setPixmap(pm);

	return true;
}
