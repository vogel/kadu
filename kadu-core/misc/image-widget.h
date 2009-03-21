/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef IMAGE_WIDGET_H
#define IMAGE_WIDGET_H

#include <QtGui/QImage>
#include <QtGui/QWidget>

#include "exports.h"

class KADUAPI ImageWidget : public QWidget
{
	QImage Image;

protected:
	virtual void paintEvent(QPaintEvent *e);

public:
	ImageWidget(QWidget *parent);
	ImageWidget(const QByteArray &image, QWidget *parent);
	void setImage(const QByteArray &image);
	void setImage(const QPixmap &image);
};

#endif // IMAGE_WIDGET_H
