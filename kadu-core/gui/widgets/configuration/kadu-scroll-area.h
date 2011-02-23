/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2008, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef KADU_SCROLL_AREA_H
#define KADU_SCROLL_AREA_H

#include <QtGui/QScrollArea>
#include <QtGui/QScrollBar>

class KaduScrollArea : public QScrollArea
{
	Q_OBJECT

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
