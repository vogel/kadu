/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010, 2011, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QDateTime>
#include <QtCore/QDir>

#include "configuration/screen-shot-configuration.h"
#include "gui/windows/message-dialog.h"

#include "screen-shot-saver.h"

ScreenShotSaver::ScreenShotSaver(QObject *parent) :
		QObject{parent},
		Size{}
{
}

ScreenShotSaver::~ScreenShotSaver()
{
}

QString ScreenShotSaver::createScreenshotPath()
{
	QString dirPath = ScreenShotConfiguration::instance()->imagePath();

	QDir dir(dirPath);
	if (!dir.exists() && !dir.mkpath(dirPath))
	{
		MessageDialog::show(KaduIcon("dialog-warning"), tr("Kadu"), tr("Unable to create direcotry %1 for storing screenshots!").arg(dirPath));
		return QString();
	}

	return QDir::cleanPath(QString("%1/%2%3.%4")
			.arg(dir.absolutePath())
			.arg(ScreenShotConfiguration::instance()->fileNamePrefix())
			.arg(QString::number(QDateTime::currentDateTime().toTime_t()))
			.arg(ScreenShotConfiguration::instance()->screenshotFileNameExtension().toLower()));
}

QString ScreenShotSaver::saveScreenShot(QPixmap pixmap)
{
	QString path = createScreenshotPath();
	if (path.isEmpty())
		return QString();

	int quality = ScreenShotConfiguration::instance()->quality();

	// do not extract qPrintable... to variable
	if (!pixmap.save(path, qPrintable(ScreenShotConfiguration::instance()->fileFormat()), quality))
	{
		MessageDialog::show(KaduIcon("dialog-warning"), tr("Kadu"), tr("Can't write file %1.\nAccess denied or other problem!").arg(path));
		return QString();
	}

	QFileInfo f(path);
	Size = f.size();

	if (Size == 0)
	{
		MessageDialog::show(KaduIcon("dialog-warning"), tr("Kadu"), tr("Screenshot %1 has 0 size!\nIt should be bigger.").arg(path));
		return QString();
	}

	return path;
}

#include "moc_screen-shot-saver.cpp"
