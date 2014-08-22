/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011, 2012 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include <QtGui/QMouseEvent>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QHBoxLayout>
#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
#include <QtX11Extras/QX11Info>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#endif

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "debug.h"

#include "gui/widgets/crop-image-widget.h"
#include "gui/widgets/screenshot-tool-box.h"

#include "screenshot-widget.h"

ScreenshotWidget::ScreenshotWidget(QWidget *parent) :
		QWidget(parent, Qt::CustomizeWindowHint | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint),
		ShotMode(ShotModeStandard)
{
	setWindowRole("kadu-screenshot");

	setFocusPolicy(Qt::StrongFocus);
#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
	// set always-on-top and force taskbar and pager skipping
	Atom win_state = XInternAtom( QX11Info::display(), "_NET_WM_STATE", False );
	Atom win_state_setting[] =
	{
		XInternAtom( QX11Info::display(), "_NET_WM_STATE_ABOVE"       , False ),
		XInternAtom( QX11Info::display(), "_NET_WM_STATE_SKIP_TASKBAR", False ),
		XInternAtom( QX11Info::display(), "_NET_WM_STATE_SKIP_PAGER"  , False )
	};
	XChangeProperty( QX11Info::display(), window()->winId(), win_state, XA_ATOM, 32, PropModeReplace, (unsigned char*)&win_state_setting, 3 );
	// prevent compositing suspension on KDE4
	setAttribute( Qt::WA_TranslucentBackground, true );
#endif

	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->setMargin(0);
	layout->setContentsMargins(0, 0, 0, 0);

	CropWidget = new CropImageWidget(this);
	connect(CropWidget, SIGNAL(pixmapCropped(QPixmap)), this, SLOT(pixmapCapturedSlot(QPixmap)));
	connect(CropWidget, SIGNAL(canceled()), this, SLOT(canceledSlot()));
	layout->addWidget(CropWidget);
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

void ScreenshotWidget::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
		CropWidget->crop();
}

void ScreenshotWidget::pixmapCapturedSlot(QPixmap pixmap)
{
	hide();

	emit pixmapCaptured(pixmap);
	deleteLater();
}

void ScreenshotWidget::canceledSlot()
{
	hide();

	emit canceled();
	deleteLater();
}

#include "moc_screenshot-widget.cpp"
