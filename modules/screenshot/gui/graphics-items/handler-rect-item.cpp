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

#include <QtGui/QPainter>

#include "handler-rect-item.h"

#define SIZE 32
#define HALF_SIZE 16

HandlerRectItem::HandlerRectItem(QGraphicsItem *parent) :
		QGraphicsObject(parent)
{
}

HandlerRectItem::~HandlerRectItem()
{
}

QRectF HandlerRectItem::boundingRect() const
{
	return QRectF(-HALF_SIZE, -HALF_SIZE, SIZE, SIZE);
}

void HandlerRectItem::paint (QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(option)
	Q_UNUSED(widget)

	painter->fillRect(QRect(-HALF_SIZE, -HALF_SIZE, SIZE, SIZE), Qt::white);

	painter->setBrush(Qt::black);
	painter->drawRect(QRect(-HALF_SIZE, -HALF_SIZE, SIZE, SIZE));
}
