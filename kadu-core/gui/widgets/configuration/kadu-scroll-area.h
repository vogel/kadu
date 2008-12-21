/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KADU_SCROLL_AREA_H
#define KADU_SCROLL_AREA_H

#include <QtGui/QScrollArea>
#include <QtGui/QScrollBar>

class KaduScrollArea : public QScrollArea
{
public:
	KaduScrollArea(QWidget *parent)
		: QScrollArea(parent)
	{
	}

	QSize sizeHint() const
	{
	    int f = 2 * frameWidth();
		QSize sz(f, f);
		sz += widget()->sizeHint();

		if (verticalScrollBarPolicy() == Qt::ScrollBarAlwaysOn)
			sz.setWidth(sz.width() + verticalScrollBar()->sizeHint().width());
		if (horizontalScrollBarPolicy() == Qt::ScrollBarAlwaysOn)
			sz.setHeight(sz.height() + horizontalScrollBar()->sizeHint().height());

		return sz;
	}
};

#endif
