/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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
		QGraphicsItem(parent), Shadow(0, 0, 0, 127)
{
}

SelectionFrameItem::~SelectionFrameItem()
{
}

void SelectionFrameItem::setSize(const QSize &size)
{
	Size = size;
}

void SelectionFrameItem::setSelection(const QRect &selection)
{
	Selection = selection;
}

QRectF SelectionFrameItem::boundingRect() const
{
	return QRect(0, 0, Size.width(), Size.height());
}

void SelectionFrameItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(option)
	Q_UNUSED(widget)

	QRect left;
	left.setLeft(0);
	left.setTop(0);
	left.setRight(Selection.left() - 1);
	left.setBottom(static_cast<int>(boundingRect().height()));
	painter->fillRect(left, Shadow);

	QRect right;
	right.setLeft(Selection.right() + 1);
	right.setTop(0);
	right.setRight(static_cast<int>(boundingRect().right()));
	right.setBottom(static_cast<int>(boundingRect().height()));
	painter->fillRect(right, Shadow);

	QRect top;
	top.setLeft(Selection.left());
	top.setTop(0);
	top.setRight(Selection.right());
	top.setBottom(Selection.top() - 1);
	painter->fillRect(top, Shadow);

	QRect bottom;
	bottom.setLeft(Selection.left());
	bottom.setTop(Selection.bottom() + 1);
	bottom.setRight(Selection.right());
	bottom.setBottom(static_cast<int>(boundingRect().bottom()));
	painter->fillRect(bottom, Shadow);
}
