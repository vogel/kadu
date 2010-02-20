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

#include <QtGui/QCursor>
#include <QtGui/QGraphicsPixmapItem>
#include <QtGui/QResizeEvent>

#include "gui/graphics-items/handler-rect-item.h"
#include "gui/graphics-items/selection-frame-item.h"

#include "crop-image-widget.h"

CropImageWidget::CropImageWidget(QWidget *parent) :
		QGraphicsView(parent), IsMouseButtonPressed(false)
{
	setContentsMargins(0, 0, 0, 0);
	setFrameShape(NoFrame);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	QGraphicsScene *graphicsScene = new QGraphicsScene(this);
	setScene(graphicsScene);

	PixmapItem = new QGraphicsPixmapItem();
	PixmapItem->setCursor(Qt::CrossCursor);
	PixmapItem->setX(0);
	PixmapItem->setY(0);

	scene()->addItem(PixmapItem);

	SelectionFrame = new SelectionFrameItem();
	SelectionFrame->setPos(0, 0);
	SelectionFrame->setSize(size());
	SelectionFrame->setCursor(Qt::SizeFDiagCursor);

	scene()->addItem(SelectionFrame);

	TopLeftHandler = new HandlerRectItem();
	TopLeftHandler->setCursor(Qt::SizeBDiagCursor);
	scene()->addItem(TopLeftHandler);

	TopHandler = new HandlerRectItem();
	TopHandler->setCursor(Qt::SizeVerCursor);
	scene()->addItem(TopHandler);

	TopRightHandler = new HandlerRectItem();
	TopRightHandler->setCursor(Qt::SizeFDiagCursor);
	scene()->addItem(TopRightHandler);

	LeftHandler = new HandlerRectItem();
	LeftHandler->setCursor(Qt::SizeHorCursor);
	scene()->addItem(LeftHandler);

	RightHandler = new HandlerRectItem();
	RightHandler->setCursor(Qt::SizeHorCursor);
	scene()->addItem(RightHandler);

	BottomLeftHandler = new HandlerRectItem();
	BottomLeftHandler->setCursor(Qt::SizeFDiagCursor);
	scene()->addItem(BottomLeftHandler);

	BottomHandler = new HandlerRectItem();
	BottomHandler->setCursor(Qt::SizeVerCursor);
	scene()->addItem(BottomHandler);

	BottomRightHandler = new HandlerRectItem();
	BottomRightHandler->setCursor(Qt::SizeBDiagCursor);
	scene()->addItem(BottomRightHandler);
}

CropImageWidget::~CropImageWidget()
{
}

void CropImageWidget::updateCropRectDisplay()
{
	QRect normalized = CropRect.normalized();
	SelectionFrame->setSelection(normalized);

	int xMiddle = (normalized.left() + normalized.right()) / 2;
	int yMiddle = (normalized.top() + normalized.bottom()) / 2;

	TopLeftHandler->setPos(normalized.left(), normalized.top());
	TopHandler->setPos(xMiddle, normalized.top());
	TopRightHandler->setPos(normalized.right(), normalized.top());
	LeftHandler->setPos(normalized.left(), yMiddle);
	RightHandler->setPos(normalized.right(), yMiddle);
	BottomLeftHandler->setPos(normalized.left(), normalized.bottom());
	BottomHandler->setPos(xMiddle, normalized.bottom());
	BottomRightHandler->setPos(normalized.right(), normalized.bottom());

	scene()->update(scene()->sceneRect());
}

void CropImageWidget::mousePressEvent(QMouseEvent *event)
{
	if (event->button() != Qt::LeftButton)
		return;

	IsMouseButtonPressed = true;

	CropRect.setTopLeft(event->pos());
	CropRect.setBottomRight(event->pos());
}

void CropImageWidget::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() != Qt::LeftButton)
		return;

	IsMouseButtonPressed = false;

	CropRect.setBottomRight(event->pos());
	updateCropRectDisplay();
}

void CropImageWidget::mouseMoveEvent(QMouseEvent *event)
{
	if (!IsMouseButtonPressed)
		return;

	CropRect.setBottomRight(event->pos());
	updateCropRectDisplay();
}

void CropImageWidget::resizeEvent(QResizeEvent *event)
{
	SelectionFrame->setSize(event->size());
    QGraphicsView::resizeEvent(event);

	scene()->setSceneRect(QRectF(0, 0, event->size().width(), event->size().height()));
}

void CropImageWidget::setPixmap(QPixmap pixmap)
{
	PixmapItem->setPixmap(pixmap);
}

void CropImageWidget::setCropRect(QRect cropRect)
{
	CropRect = cropRect.normalized();
	updateCropRectDisplay();
}
