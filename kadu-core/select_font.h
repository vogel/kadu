/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SELECT_FONT_H
#define SELECT_FONT_H

#include <qfont.h>
#include <qhbox.h>

class QLineEdit;

class SelectFont : public QHBox
{
	Q_OBJECT

	QFont currentFont;
	QLineEdit *fontEdit;

private slots:
	void onClick();

public:
	SelectFont(QWidget *parent = 0,  const char *name = 0);
	const QFont &font() const;
	void setFont(const QFont &font);

signals:
	void fontChanged(QFont font);

};

#endif // SELECT_FONT_H
