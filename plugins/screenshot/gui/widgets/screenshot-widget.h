/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef SCREENSHOT_WIDGET_H
#define SCREENSHOT_WIDGET_H

#include <QtGui/QPixmap>
#include <QtGui/QWidget>

#include "screenshot-mode.h"

class CropImageWidget;

class ScreenshotWidget : public QWidget
{
	Q_OBJECT

	CropImageWidget *CropWidget;

	ScreenShotMode ShotMode;
	QRect ShotRegion;

private slots:
	void pixmapCapturedSlot(QPixmap pixmap);
	void canceledSlot();

protected:
	virtual void keyPressEvent(QKeyEvent *event);

public:
	explicit ScreenshotWidget(QWidget *parent = 0);
	virtual ~ScreenshotWidget();

	void setShotMode(ScreenShotMode shotMode);
	void setPixmap(QPixmap pixmap);

signals:
	void pixmapCaptured(QPixmap pixmap);
	void canceled();

};

#endif // SCREENSHOT_WIDGET_H
