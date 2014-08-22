/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

/* Some code in this file heavily based on code from Psi+ project.
 * Original copyright below. */

/*
 * advwidget.cpp - AdvancedWidget template class
 * Copyright (C) 2005-2007  Michail Pishchagin
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

#include <QtCore/QVariant>
#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>
#include <QtGui/QWidget>

#ifdef Q_OS_MAC
#include <QtGui/QMainWindow>
#endif

#include "misc/misc.h"
#include "misc/variant-wrapper.h"

#include "window-geometry-manager.h"

static const int NormalGeometryIndex = 0;
static const int FrameGeometryIndex = 1;
static const int ScreenIndex = 2;
static const int MaximizedIndex = 3;
static const int FullscreenIndex = 4;
static const int IndexCount = 5;

WindowGeometryManager::WindowGeometryManager(VariantWrapper *variantWrapper, const QRect &defaultGeometry, QWidget *window) :
		QObject(window), MyVariantWrapper(variantWrapper), DefaultGeometry(defaultGeometry)
{
	Q_ASSERT(window);
	Q_ASSERT(window->isWindow());
	Q_ASSERT(MyVariantWrapper);

	Timer.setInterval(100);
	Timer.setSingleShot(true);
	connect(&Timer, SIGNAL(timeout()), SLOT(saveGeometry()));

	restoreGeometry();
	window->installEventFilter(this);
}

WindowGeometryManager::~WindowGeometryManager()
{
	delete MyVariantWrapper;
	MyVariantWrapper = 0;
}

bool WindowGeometryManager::eventFilter(QObject *watched, QEvent *event)
{
	if (watched == parent())
	{
		QWidget *parentWidget = qobject_cast<QWidget *>(parent());
		Q_ASSERT(parentWidget);

		if (event->type() == QEvent::Move || event->type() == QEvent::Resize)
		{
			if (parentWidget->isWindow())
			{
				Qt::WindowStates ws = parentWidget->windowState();
				if(!(ws & Qt::WindowMaximized) && !(ws & Qt::WindowFullScreen))
					NormalGeometry = parentWidget->normalGeometry();
				Timer.start();
			}
		}
		else if (event->type() == QEvent::ParentChange)
		{
			if (parentWidget->isWindow())
				restoreGeometry();
		}

		return false;
	}

	return QObject::eventFilter(watched, event);
}

void WindowGeometryManager::saveGeometry()
{
	QWidget *parentWidget = qobject_cast<QWidget *>(parent());
	Q_ASSERT(parentWidget);

#ifdef Q_OS_MAC
	/* Dorr: workaround for Qt window geometry bug when unified toolbars enabled */
	/* TODO: Check if this is still needed. If not, drop all 4 calls in this file. */
	/* TODO: Check if we can benefit from the OS X workaround as used in QWidget::saveGeometry()
	 *       implementation. If so, use it here. */
	if (QMainWindow *mainWindow = qobject_cast<QMainWindow *>(parentWidget))
		mainWindow->setUnifiedTitleAndToolBarOnMac(false);
#endif

	bool isMaximized = parentWidget->windowState() & Qt::WindowMaximized;
	QStringList configuration;
	//if window is maximized normalGeometry() returns null rect. So in this case we use cached geometry
	configuration.insert(NormalGeometryIndex, rectToString(isMaximized ? NormalGeometry : parentWidget->normalGeometry()));
	configuration.insert(FrameGeometryIndex, rectToString(parentWidget->frameGeometry()));
	configuration.insert(ScreenIndex, QString::number(QApplication::desktop()->screenNumber(parentWidget)));
	configuration.insert(MaximizedIndex, QString::number(int(isMaximized)));
	configuration.insert(FullscreenIndex, QString::number(int(bool(parentWidget->windowState() & Qt::WindowFullScreen))));

	MyVariantWrapper->set(configuration.join(":"));

#ifdef Q_OS_MAC
	/* Dorr: workaround for Qt window geometry bug when unified toolbars enabled */
	if (QMainWindow *mainWindow = qobject_cast<QMainWindow *>(parentWidget))
		mainWindow->setUnifiedTitleAndToolBarOnMac(true);
#endif
}

void WindowGeometryManager::restoreGeometry()
{
	QWidget *parentWidget = qobject_cast<QWidget *>(parent());
	Q_ASSERT(parentWidget);

#ifdef Q_OS_MAC
	/* Dorr: workaround for Qt window geometry bug when unified toolbars enabled */
	if (QMainWindow *mainWindow = qobject_cast<QMainWindow *>(parentWidget))
		mainWindow->setUnifiedTitleAndToolBarOnMac(false);
#endif

	QString configurationString = MyVariantWrapper->get().toString();
	QStringList configuration = configurationString.split(':');
	if (configuration.count() != IndexCount)
	{
		QRect rect = stringToRect(configurationString);
		if (!rect.isValid())
			rect = DefaultGeometry;

		rect = properGeometry(rect);
		parentWidget->move(rect.topLeft());
		parentWidget->resize(rect.size());
	}
	else
	{
		// if future Qt versions drop support for restoring from this format old options files
		// would break anyway. If we want to (e.g. to add other features) we can also reimplement
		// restoring any other way without breaking the options format at all.
		// and this way we are sure no Qt version the user happens to have installed writes some
		// newer version of the format that older Qts can't restore from.

		QByteArray array;
		QDataStream stream(&array, QIODevice::WriteOnly);
		stream.setVersion(QDataStream::Qt_4_0);
		const quint32 magicNumber = 0x1D9D0CB;
		const quint16 majorVersion = 1;
		const quint16 minorVersion = 0;

		NormalGeometry = stringToRect(configuration.at(NormalGeometryIndex));

		stream << magicNumber
				<< majorVersion
				<< minorVersion
				<< stringToRect(configuration.at(FrameGeometryIndex))
				<< NormalGeometry
				<< qint32(configuration.at(ScreenIndex).toInt())
				<< quint8(bool(configuration.at(MaximizedIndex).toInt()))
				<< quint8(bool(configuration.at(FullscreenIndex).toInt()));

		parentWidget->restoreGeometry(array);
	}

#ifdef Q_OS_MAC
	/* Dorr: workaround for Qt window geometry bug when unified toolbars enabled */
	if (QMainWindow *mainWindow = qobject_cast<QMainWindow *>(parentWidget))
		mainWindow->setUnifiedTitleAndToolBarOnMac(true);
#endif
}

#include "moc_window-geometry-manager.cpp"
