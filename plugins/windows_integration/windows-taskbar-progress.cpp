/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "windows-taskbar-progress.h"

#include "file-transfer/file-transfer-manager.h"

#include <QtWidgets/QWidget>

#ifdef Q_OS_WIN
#	include <QtWinExtras/QtWinExtras>
#endif

WindowsTaskbarProgress::WindowsTaskbarProgress(FileTransferManager *fileTransferManager, QWidget *parent) :
		QObject{parent}
{
	parent->window()->winId(); // force windowHandle() to be valid

#ifdef Q_OS_WIN
	auto button = new QWinTaskbarButton{parent->window()};
	button->setWindow(parent->window()->windowHandle());

	m_taskbarProgress = button->progress();
	m_taskbarProgress->setRange(0, 100);
#endif

	connect(fileTransferManager, SIGNAL(totalProgressChanged(int)), this, SLOT(progressChanged(int)));
	progressChanged(fileTransferManager->totalProgress());
}

WindowsTaskbarProgress::~WindowsTaskbarProgress()
{
}

void WindowsTaskbarProgress::progressChanged(int progress)
{
#ifdef Q_OS_WIN
	if (progress < 100)
	{
		m_taskbarProgress->setVisible(true);
		m_taskbarProgress->setValue(progress);
	}
	else
		m_taskbarProgress->setVisible(false);
#else
	Q_UNUSED(progress);
	Q_UNUSED(m_taskbarProgress);
#endif
}

#include "moc_windows-taskbar-progress.cpp"
