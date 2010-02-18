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

#include <QtGui/QGraphicsScene>
#include <QtGui/QPainter>
#include <QtGui/QStyleOption>

#include "selection-frame-item.h"

SelectionFrameItem::SelectionFrameItem(QGraphicsItem *parent) :
		QGraphicsItem(parent)
{
}

SelectionFrameItem::~SelectionFrameItem()
{
}

void SelectionFrameItem::setSize(QSize size)
{
	Size = size;
}

QRectF SelectionFrameItem::boundingRect() const
{
	return QRect(0, 0, Size.width(), Size.height());
}

void SelectionFrameItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(option)
	Q_UNUSED(widget)

	QRect selectedRect(40, 40, 80, 80);

	QColor shadow(0, 0, 0, 127);

	QRect left;
	left.setLeft(0);
	left.setTop(0);
	left.setRight(selectedRect.left() - 1);
	left.setBottom(boundingRect().height());
	painter->fillRect(left, shadow);

	QRect right;
	right.setLeft(selectedRect.right() + 1);
	right.setTop(0);
	right.setRight(boundingRect().right());
	right.setBottom(boundingRect().height());
	painter->fillRect(right, shadow);

	QRect top;
	top.setLeft(selectedRect.left());
	top.setTop(0);
	top.setRight(selectedRect.right());
	top.setBottom(selectedRect.top() - 1);
	painter->fillRect(top, shadow);

	QRect bottom;
	bottom.setLeft(selectedRect.left());
	bottom.setTop(selectedRect.bottom() + 1);
	bottom.setRight(selectedRect.right());
	bottom.setBottom(boundingRect().bottom());
	painter->fillRect(bottom, shadow);
}
