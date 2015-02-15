/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef SELECTION_FRAME_ITEM_H
#define SELECTION_FRAME_ITEM_H

#include <QtWidgets/QGraphicsItem>

class SelectionFrameItem : public QGraphicsItem
{
	QColor Shadow;
	QSize Size;
	QRect Selection;

protected:
	virtual QRectF boundingRect() const;
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

public:
	explicit SelectionFrameItem(QGraphicsItem *parent = 0);
	virtual ~SelectionFrameItem();

	void setSize(const QSize &size);
	void setSelection(const QRect &selection);

};

#endif // SELECTION_FRAME_H
