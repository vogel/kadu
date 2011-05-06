/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QGraphicsSceneMouseEvent>
#include <QtGui/QPainter>

#include "handler-rect-item.h"

HandlerRectItem::HandlerRectItem(HandlerType type, int size, QGraphicsItem *parent) :
		QGraphicsWidget(parent), Type(type), Size(size), IsMouseButtonPressed(false)
{
}

HandlerRectItem::~HandlerRectItem()
{
}

void HandlerRectItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (Qt::LeftButton != event->button())
	{
		event->ignore();
		return;
	}

	grabMouse();

	IsMouseButtonPressed = true;
	event->accept();
}

void HandlerRectItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	if ((Qt::LeftButton != event->button()) || !IsMouseButtonPressed)
	{
		event->ignore();
		return;
	}

	ungrabMouse();

	IsMouseButtonPressed = false;
	event->accept();
}

void HandlerRectItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (!IsMouseButtonPressed)
	{
		event->ignore();
		return;
	}

	emit movedTo(Type, event->scenePos().x(), event->scenePos().y());
}

QRectF HandlerRectItem::boundingRect() const
{
	return QRectF(0, 0, Size, Size);
}

void HandlerRectItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(option)
	Q_UNUSED(widget)

	painter->fillRect(QRect(0, 0, Size, Size), Qt::white);

	painter->setPen(Qt::black);
	painter->drawRect(QRect(0, 0, Size, Size));
}
