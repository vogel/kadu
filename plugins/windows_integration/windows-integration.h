/*
 * %kadu copyright begin%
 * Copyright 2016 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class FileTransferManager;
class PluginInjectedFactory;
class MainWindowRepository;
class StatusContainerManager;

class WindowsIntegration : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit WindowsIntegration(QObject *parent = nullptr);
	virtual ~WindowsIntegration();

private:
	QPointer<FileTransferManager> m_fileTransferManager;
	QPointer<PluginInjectedFactory> m_pluginInjectedFactory;
	QPointer<MainWindowRepository> m_mainWindowRepository;
	QPointer<StatusContainerManager> m_statusContainerManager;

private slots:
	INJEQT_SET void setFileTransferManager(FileTransferManager *fileTransferManager);
	INJEQT_SET void setPluginInjectedFactory(PluginInjectedFactory *pluginInjectedFactory);
	INJEQT_SET void setMainWindowRepository(MainWindowRepository *mainWindowRepository);
	INJEQT_SET void setStatusContainerManager(StatusContainerManager *statusContainerManager);
	INJEQT_INIT void init();

	void mainWindowAdded(QWidget *mainWindow);

};
