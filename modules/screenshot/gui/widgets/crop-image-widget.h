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

class QGraphicsPixmapItem;

class CropImageWidget : public QGraphicsView
{
	Q_OBJECT

	QGraphicsPixmapItem *PixmapItem;

public:
	explicit CropImageWidget(QWidget *parent = 0);
	virtual ~CropImageWidget();

	void setPixmap(QPixmap pixmap);

};

#endif // CROP_IMAGE_WIDGET_H
