/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QBuffer>
#include <QtCore/QTimer>
#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>
#include <QtGui/QHBoxLayout>
#include <QtGui/QMouseEvent>

#include "configuration/configuration-file.h"
#include "debug.h"

#include "gui/widgets/crop-image-widget.h"
#include "gui/widgets/screenshot-tool-box.h"

#include "screenshot-widget.h"

ScreenshotWidget::ScreenshotWidget(QWidget *parent) :
		QWidget(parent), ButtonPressed(false), ShotMode(ShotModeStandard)
{
	setWindowFlags(Qt::Tool | Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->setMargin(0);
	layout->setContentsMargins(0, 0, 0, 0);

	CropWidget = new CropImageWidget(this);
	layout->addWidget(CropWidget);

	ToolBox = new ScreenshotToolBox(this);
	ToolBoxTimer = new QTimer(this);
	connect(ToolBox, SIGNAL(timeout()), this, SLOT(updateToolBoxHint()));
}

ScreenshotWidget::~ScreenshotWidget()
{
}

void ScreenshotWidget::setShotMode(ScreenShotMode shotMode)
{
	ShotMode = shotMode;
}

void ScreenshotWidget::setPixmap(QPixmap pixmap)
{
	CropWidget->setPixmap(pixmap);

	resize(pixmap.size());
}

void ScreenshotWidget::updateToolBoxHint()
{
	QBuffer buffer;

	QRect reg = ShotRegion;
	reg = reg.normalized();

	QPixmap shot = QPixmap::grabWindow(winId(), reg.x(), reg.y(), reg.width(), reg.height());

	// TODO: cache + use configurationUpdated
	const char *format = config_file.readEntry("ScreenShot", "fileFormat", "PNG").toAscii();
	int quality = config_file.readNumEntry("ScreenShot", "quality", -1);
	bool ret = shot.save(&buffer, format, quality);

	if (ret)
		ToolBox->setFileSize(QString::number(buffer.size()/1024) + " KB");
}

void ScreenshotWidget::mousePressEvent(QMouseEvent *e)
{
	kdebugf();

	if (e->button() != Qt::LeftButton)
		return;

	if (ShotMode == ShotModeSingleWindow)
	{
		releaseMouse();
		releaseKeyboard();

		hide();
		update();

		QTimer::singleShot(100, this, SLOT(takeWindowShot_Step2()));
	}
	else
	{
		ShotRegion = QRect(e->pos(), e->pos());
		ButtonPressed = true;

		int x = e->pos().x() + 50;
		int y = e->pos().y() + 50;

		QRect screen = QApplication::desktop()->screenGeometry();
		if (x + 150 > screen.width())
			x -= 150;

		if (y + 100 > screen.height())
			y -= 100;

		ToolBox->move(x, y);

		ToolBox->setGeometry("0x0");
		ToolBox->setFileSize("0 KB");
		ToolBox->show();
		ToolBoxTimer->start(1000);
	}
}

void ScreenshotWidget::mouseReleaseEvent(QMouseEvent *e)
{
	kdebugf();

	if (!ButtonPressed)
		return;

	ToolBoxTimer->stop();
	ToolBox->hide();

	// Uwalnianie myszki, klawiatury
	ButtonPressed = false;
	releaseMouse();
	releaseKeyboard();

	// Normalizowanie prostok�ta do zrzutu
	ShotRegion.setBottomRight(e->pos());
	ShotRegion = ShotRegion.normalized();

	// Zrzut
	ShowPaintRect = false;
	repaint();
	qApp->processEvents();

	// TODO: rotfl... we have pixmap variable
	QPixmap shot = QPixmap::grabWindow(winId(), ShotRegion.x(), ShotRegion.y(), ShotRegion.width(), ShotRegion.height());

	// Chowanie widgeta zrzutu i przywr�cenie kursora.
	hide();
	QApplication::restoreOverrideCursor();

	emit pixmapCaptured(shot);
}

void ScreenshotWidget::mouseMoveEvent(QMouseEvent *e)
{
	kdebugf();
	if (!ButtonPressed)
		return;

	ShotRegion.setBottomRight(e->pos());

	QRect reg = ShotRegion;
	reg = reg.normalized();

	ToolBox->setGeometry(
		QString("%1x%2")
			.arg(QString::number(reg.width()))
			.arg(QString::number(reg.height()))
		);

	ShowPaintRect = true;
	repaint();
}

void ScreenshotWidget::keyPressEvent(QKeyEvent* e)
{
	kdebugf();

	if (e->key() == Qt::Key_Escape)
	{
		releaseMouse();
		releaseKeyboard();
		hide();
	}
}
