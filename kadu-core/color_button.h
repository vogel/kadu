/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef COLOR_BUTTON_H
#define COLOR_BUTTON_H

#include <qcolor.h>
#include <qpushbutton.h>

/**
    Kontrolka do wyboru koloru
**/
class ColorButton : public QPushButton
{
	Q_OBJECT

	QColor currentColor;

public:
	ColorButton(QWidget *parent = 0, const char * name = 0);
	const QColor &color() const;
	bool setColor(const QColor &color);

private slots:
	void onClick();

signals:
	void changed(const QColor &color);

};

#endif // COLOR_BUTTON_H
