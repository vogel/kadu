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

#ifndef CROP_IMAGE_WIDGET_H
#define CROP_IMAGE_WIDGET_H

#include <QtGui/QGraphicsView>

#include "gui/graphics-items/handler-rect-item.h"

class QGraphicsPixmapItem;

class ScreenshotToolBox;
class SelectionFrameItem;

class CropImageWidget : public QGraphicsView
{
	Q_OBJECT

	QGraphicsPixmapItem *PixmapItem;
	SelectionFrameItem *SelectionFrame;
	HandlerRectItem *TopLeftHandler;
	HandlerRectItem *TopHandler;
	HandlerRectItem *TopRightHandler;
	HandlerRectItem *LeftHandler;
	HandlerRectItem *RightHandler;
	HandlerRectItem *BottomLeftHandler;
	HandlerRectItem *BottomHandler;
	HandlerRectItem *BottomRightHandler;
	QGraphicsProxyWidget *ToolBoxProxy;
	ScreenshotToolBox *ToolBox;

	QTimer *ToolBoxTimer;

	QPoint NewTopLeft;
	QRect CropRect;
	bool IsMouseButtonPressed;
	bool WasDoubleClick;

	void normalizeCropRect();
	void updateCropRectDisplay();

	QPixmap croppedPixmap();

private slots:
	void crop();
	void updateToolBoxFileSizeHint();
	void handlerMovedTo(HandlerType type, int x, int y);

protected:
    virtual void keyPressEvent(QKeyEvent *event);

	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);
	virtual void mouseMoveEvent(QMouseEvent *event);

	virtual void resizeEvent(QResizeEvent *event);
	virtual void showEvent(QShowEvent *event);

public:
	explicit CropImageWidget(QWidget *parent = 0);
	virtual ~CropImageWidget();

	void setPixmap(QPixmap pixmap);
	void setCropRect(const QRect &cropRect);

signals:
	void pixmapCropped(QPixmap pixmap);
	void canceled();

};

#endif // CROP_IMAGE_WIDGET_H
