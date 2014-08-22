/*
 * %kadu copyright begin%
 * Copyright 2008, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
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

#ifndef HANDLER_RECT_ITEM_H
#define HANDLER_RECT_ITEM_H

#include <QtGui/QGraphicsWidget>

enum HandlerType
{
	HandlerTopLeft,
	HandlerTop,
	HandlerTopRight,
	HandlerLeft,
	HandlerRight,
	HandlerBottomLeft,
	HandlerBottom,
	HandlerBottomRight
};

class HandlerRectItem : public QGraphicsWidget
{
	Q_OBJECT

	HandlerType Type;
	int Size;

	bool IsMouseButtonPressed;
	QPointF LastMousePosition;

protected:
	virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

	virtual QRectF boundingRect() const;
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

public:
	explicit HandlerRectItem(HandlerType type, int size = 10, QGraphicsItem *parent = 0);
	virtual ~HandlerRectItem();

signals:
	void movedTo(HandlerType type, int x, int y);

};

#endif // HANDLER_RECT_ITEM_H
