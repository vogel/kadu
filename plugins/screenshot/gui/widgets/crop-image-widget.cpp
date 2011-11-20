/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#include <math.h>

#include <QtCore/QBuffer>
#include <QtCore/QTimer>
#include <QtGui/QCursor>
#include <QtGui/QGraphicsPixmapItem>
#include <QtGui/QGraphicsProxyWidget>
#include <QtGui/QResizeEvent>

#include "gui/graphics-items/selection-frame-item.h"
#include "gui/widgets/screenshot-tool-box.h"

#include "crop-image-widget.h"

#define HANDLER_SIZE 10
#define HANDLER_HALF_SIZE (HANDLER_SIZE/2)
#define TOOLBOX_PADDING 10

CropImageWidget::CropImageWidget(QWidget *parent) :
		QGraphicsView(parent), IsMouseButtonPressed(false)
{
	setContentsMargins(0, 0, 0, 0);
	setFrameShape(NoFrame);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setInteractive(true);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	QGraphicsScene *graphicsScene = new QGraphicsScene(this);
	setScene(graphicsScene);

	PixmapItem = new QGraphicsPixmapItem();
	PixmapItem->setCursor(Qt::CrossCursor);
	PixmapItem->setPos(0, 0);

	scene()->addItem(PixmapItem);

	SelectionFrame = new SelectionFrameItem();
	SelectionFrame->setPos(0, 0);
	SelectionFrame->setSize(size());

	scene()->addItem(SelectionFrame);

	ToolBox = new ScreenshotToolBox();
	connect(ToolBox, SIGNAL(crop()), this, SLOT(crop()));
	connect(ToolBox, SIGNAL(cancel()), this, SIGNAL(canceled()));

	ToolBoxTimer = new QTimer(this);
	connect(ToolBoxTimer, SIGNAL(timeout()), this, SLOT(updateToolBoxFileSizeHint()));
	ToolBoxTimer->start(1000);

	ToolBoxProxy = new QGraphicsProxyWidget();
	ToolBoxProxy->setWidget(ToolBox);
	scene()->addItem(ToolBoxProxy);

	TopLeftHandler = new HandlerRectItem(HandlerTopLeft, HANDLER_SIZE);
	TopLeftHandler->setCursor(Qt::SizeFDiagCursor);
	connect(TopLeftHandler, SIGNAL(movedTo(HandlerType,int,int)), this, SLOT(handlerMovedTo(HandlerType,int,int)));
	scene()->addItem(TopLeftHandler);

	TopHandler = new HandlerRectItem(HandlerTop, HANDLER_SIZE);
	TopHandler->setCursor(Qt::SizeVerCursor);
	connect(TopHandler, SIGNAL(movedTo(HandlerType,int,int)), this, SLOT(handlerMovedTo(HandlerType,int,int)));
	scene()->addItem(TopHandler);

	TopRightHandler = new HandlerRectItem(HandlerTopRight, HANDLER_SIZE);
	TopRightHandler->setCursor(Qt::SizeBDiagCursor);
	connect(TopRightHandler, SIGNAL(movedTo(HandlerType,int,int)), this, SLOT(handlerMovedTo(HandlerType,int,int)));
	scene()->addItem(TopRightHandler);

	LeftHandler = new HandlerRectItem(HandlerLeft, HANDLER_SIZE);
	LeftHandler->setCursor(Qt::SizeHorCursor);
	connect(LeftHandler, SIGNAL(movedTo(HandlerType,int,int)), this, SLOT(handlerMovedTo(HandlerType,int,int)));
	scene()->addItem(LeftHandler);

	RightHandler = new HandlerRectItem(HandlerRight, HANDLER_SIZE);
	RightHandler->setCursor(Qt::SizeHorCursor);
	connect(RightHandler, SIGNAL(movedTo(HandlerType,int,int)), this, SLOT(handlerMovedTo(HandlerType,int,int)));
	scene()->addItem(RightHandler);

	BottomLeftHandler = new HandlerRectItem(HandlerBottomLeft, HANDLER_SIZE);
	BottomLeftHandler->setCursor(Qt::SizeBDiagCursor);
	connect(BottomLeftHandler, SIGNAL(movedTo(HandlerType,int,int)), this, SLOT(handlerMovedTo(HandlerType,int,int)));
	scene()->addItem(BottomLeftHandler);

	BottomHandler = new HandlerRectItem(HandlerBottom, HANDLER_SIZE);
	BottomHandler->setCursor(Qt::SizeVerCursor);
	connect(BottomHandler, SIGNAL(movedTo(HandlerType,int,int)), this, SLOT(handlerMovedTo(HandlerType,int,int)));
	scene()->addItem(BottomHandler);

	BottomRightHandler = new HandlerRectItem(HandlerBottomRight, HANDLER_SIZE);
	BottomRightHandler->setCursor(Qt::SizeFDiagCursor);
	connect(BottomRightHandler, SIGNAL(movedTo(HandlerType,int,int)), this, SLOT(handlerMovedTo(HandlerType,int,int)));
	scene()->addItem(BottomRightHandler);

	CropRect.setTopLeft(QPoint(0, 0));
	CropRect.setSize(size());
	updateCropRectDisplay();
}

CropImageWidget::~CropImageWidget()
{
}

void CropImageWidget::normalizeCropRect()
{
	CropRect = CropRect.normalized();
	updateCropRectDisplay();
}

void CropImageWidget::updateCropRectDisplay()
{
	QRect normalized = CropRect.normalized();
	SelectionFrame->setSelection(normalized);

	// workaround for wrong cursor on negative coordinates
	TopLeftHandler->setPos(CropRect.left() - HANDLER_HALF_SIZE, CropRect.top() - HANDLER_HALF_SIZE);
	TopHandler->setPos(normalized.center().x() - HANDLER_HALF_SIZE, CropRect.top() - HANDLER_HALF_SIZE);
	TopRightHandler->setPos(CropRect.right() - HANDLER_HALF_SIZE, CropRect.top() - HANDLER_HALF_SIZE);
	LeftHandler->setPos(CropRect.left() - HANDLER_HALF_SIZE, normalized.center().y() - HANDLER_HALF_SIZE);
	RightHandler->setPos(CropRect.right() - HANDLER_HALF_SIZE, normalized.center().y() - HANDLER_HALF_SIZE);
	BottomLeftHandler->setPos(CropRect.left() - HANDLER_HALF_SIZE, CropRect.bottom() - HANDLER_HALF_SIZE);
	BottomHandler->setPos(normalized.center().x() - HANDLER_HALF_SIZE, CropRect.bottom() - HANDLER_HALF_SIZE);
	BottomRightHandler->setPos(CropRect.right() - HANDLER_HALF_SIZE, CropRect.bottom() - HANDLER_HALF_SIZE);

	ToolBox->setGeometry(QString("%1x%2").arg(normalized.width()).arg(normalized.height()));

	int left = normalized.right() + TOOLBOX_PADDING;
	if (left + ToolBox->width() > width())
		left = normalized.left() - TOOLBOX_PADDING - ToolBox->width();
	if (left < 0)
		left = normalized.center().x() - ToolBox->width() / 2;

	int top = normalized.bottom() + TOOLBOX_PADDING;
	if (top + ToolBox->height() > height())
		top = normalized.top() - TOOLBOX_PADDING - ToolBox->height();
	if (top < 0)
		top = normalized.center().y() - ToolBox->height() / 2;

	ToolBoxProxy->setPos(left, top);

	scene()->update(scene()->sceneRect());
}

QPixmap CropImageWidget::croppedPixmap()
{
	if (CropRect.normalized().isEmpty())
		return QPixmap();
	return PixmapItem->pixmap().copy(CropRect.normalized());
}

void CropImageWidget::handlerMovedTo(HandlerType type, int x, int y)
{
	if (type == HandlerTopLeft || type == HandlerTop || type == HandlerTopRight)
		CropRect.setTop(y);
	else if (type == HandlerBottomLeft || type == HandlerBottom || type == HandlerBottomRight)
		CropRect.setBottom(y);

	if (type == HandlerTopLeft || type == HandlerLeft || type == HandlerBottomLeft)
		CropRect.setLeft(x);
	else if (type == HandlerTopRight || type == HandlerRight || type == HandlerBottomRight)
		CropRect.setRight(x);

	updateCropRectDisplay();
}

void CropImageWidget::crop()
{
	emit pixmapCropped(croppedPixmap());
}

void CropImageWidget::updateToolBoxFileSizeHint()
{
	if (CropRect.normalized() == OldCropRect)
		return;
	OldCropRect = CropRect.normalized();

	QBuffer buffer;
	QPixmap pixmap = croppedPixmap();

	if (pixmap.isNull())
	{
		ToolBox->setFileSize("0 KiB");
		return;
	}

	bool ret = pixmap.save(&buffer, "png");

	if (ret)
		ToolBox->setFileSize(QString::number(ceil(1.0 * buffer.size() / 1024.0)) + " KiB");
}

void CropImageWidget::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Escape)
	{
		emit canceled();
		e->accept();
	}
	else
		QWidget::keyPressEvent(e);
}

void CropImageWidget::mousePressEvent(QMouseEvent *event)
{
	QGraphicsView::mousePressEvent(event);
	if (event->isAccepted())
		return;

	if (event->button() != Qt::LeftButton)
		return;

	IsMouseButtonPressed = true;
	WasDoubleClick = false;

	NewTopLeft = event->pos();

	updateCropRectDisplay();
}

void CropImageWidget::mouseReleaseEvent(QMouseEvent *event)
{
	QGraphicsView::mouseReleaseEvent(event);
	normalizeCropRect();

	if (!IsMouseButtonPressed)
		return;

	if (event->button() != Qt::LeftButton)
		return;

	IsMouseButtonPressed = false;

	CropRect.setTopLeft(NewTopLeft);
	CropRect.setBottomRight(event->pos());
	normalizeCropRect();
	updateToolBoxFileSizeHint();
}

void CropImageWidget::mouseMoveEvent(QMouseEvent *event)
{
	QGraphicsView::mouseMoveEvent(event);

	if (!IsMouseButtonPressed)
		return;

	CropRect.setTopLeft(NewTopLeft);
	CropRect.setBottomRight(event->pos());
	normalizeCropRect();
}

void CropImageWidget::resizeEvent(QResizeEvent *event)
{
	SelectionFrame->setSize(event->size());
    QGraphicsView::resizeEvent(event);

	scene()->setSceneRect(QRectF(0, 0, event->size().width(), event->size().height()));
}

void CropImageWidget::showEvent(QShowEvent *event)
{
    QGraphicsView::showEvent(event);

	setCropRect(frameGeometry());
}

void CropImageWidget::setPixmap(QPixmap pixmap)
{
	PixmapItem->setPixmap(pixmap);
}

void CropImageWidget::setCropRect(const QRect &cropRect)
{
	CropRect = cropRect.normalized();
	updateCropRectDisplay();
}
