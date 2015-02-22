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

#include "taskbar-progress.h"

#include "file-transfer/file-transfer-manager.h"

#include <QtWidgets/QWidget>

#ifdef Q_OS_WIN
#	include <QtWinExtras/QtWinExtras>
#endif

TaskbarProgress::TaskbarProgress(FileTransferManager *fileTransferManager, QWidget *parent) :
		QObject{parent},
		m_taskbarProgress{nullptr}
{
#ifdef Q_OS_WIN
	parent->window()->winId(); // force windowHandle() to be valid

	auto button = new QWinTaskbarButton{parent->window()};
	button->setWindow(parent->window()->windowHandle());

	m_taskbarProgress = button->progress();
	static_cast<QWinTaskbarProgress *>(m_taskbarProgress)->setRange(0, 100);

	connect(fileTransferManager, SIGNAL(totalProgressChanged(int)), this, SLOT(progressChanged(int)));
	progressChanged(fileTransferManager->totalProgress());
#else
	Q_UNUSED(fileTransferManager);
#endif
}

TaskbarProgress::~TaskbarProgress()
{
}

void TaskbarProgress::progressChanged(int progress)
{
	if (!m_taskbarProgress)
		return;

#ifdef Q_OS_WIN
	if (progress < 100)
	{
		static_cast<QWinTaskbarProgress *>(m_taskbarProgress)->setVisible(true);
		static_cast<QWinTaskbarProgress *>(m_taskbarProgress)->setValue(progress);
	}
	else
		static_cast<QWinTaskbarProgress *>(m_taskbarProgress)->setVisible(false);
#else
	Q_UNUSED(progress);
#endif
}

#include "moc_taskbar-progress.cpp"