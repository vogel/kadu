/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QPainter>

#include "image-widget.h"

ImageWidget::ImageWidget(QWidget *parent)
	: QWidget(parent), Image()
{
}

ImageWidget::ImageWidget(const QByteArray &image, QWidget *parent)
	: QWidget(parent), Image(image.data())
{
	setMinimumSize(Image.width(), Image.height());
}

void ImageWidget::setImage(const QByteArray &image)
{
	Image.loadFromData(image);
	setMinimumSize(Image.width(), Image.height());
}

void ImageWidget::setImage(const QPixmap &image)
{
	Image = image.toImage();
	setMinimumSize(Image.width(), Image.height());
}

void ImageWidget::paintEvent(QPaintEvent *)
{
	if (!Image.isNull())
	{
		QPainter p(this);
		p.drawImage(0,0,Image);
	}
}
