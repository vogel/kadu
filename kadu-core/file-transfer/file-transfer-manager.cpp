/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2010, 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2012, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "file-transfer-manager.h"

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "chat/chat-manager.h"
#include "chat/chat.h"
#include "chat/type/chat-type-contact.h"
#include "configuration/configuration-manager.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "contacts/contact-set.h"
#include "core/injected-factory.h"
#include "file-transfer/file-transfer-direction.h"
#include "file-transfer/file-transfer-handler-manager.h"
#include "file-transfer/file-transfer-notifications.h"
#include "file-transfer/file-transfer-status.h"
#include "file-transfer/file-transfer.h"
#include "file-transfer/gui/file-transfer-actions.h"
#include "file-transfer/gui/file-transfer-window.h"
#include "file-transfer/outgoing-file-transfer-handler.h"
#include "file-transfer/stream-incoming-file-transfer-handler.h"
#include "file-transfer/url-incoming-file-transfer-handler.h"
#include "gui/widgets/chat-widget/chat-widget-manager.h"
#include "gui/widgets/chat-widget/chat-widget-repository.h"
#include "gui/widgets/chat-widget/chat-widget.h"
#include "gui/windows/kadu-window-service.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/message-dialog.h"
#include "icons/icons-manager.h"
#include "notification/notification-callback-repository.h"
#include "notification/notification-event-repository.h"
#include "notification/notification-manager.h"
#include "protocols/protocol.h"
#include "protocols/services/file-transfer-service.h"
#include "storage/storage-point.h"
#include "activate.h"

#include <QtCore/QFileInfo>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

FileTransferManager::FileTransferManager(QObject *parent) :
		SimpleManager<FileTransfer>{parent},
		m_totalProgress{100}
{
}

FileTransferManager::~FileTransferManager()
{
}

void FileTransferManager::setAccountManager(AccountManager *accountManager)
{
	m_accountManager = accountManager;
}

void FileTransferManager::setChatManager(ChatManager *chatManager)
{
	m_chatManager = chatManager;
}

void FileTransferManager::setChatWidgetRepository(ChatWidgetRepository *chatWidgetRepository)
{
	m_chatWidgetRepository = chatWidgetRepository;
}

void FileTransferManager::setConfigurationManager(ConfigurationManager *configurationManager)
{
	m_configurationManager = configurationManager;
}

void FileTransferManager::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void FileTransferManager::setFileTransferActions(FileTransferActions *fileTransferActions)
{
	m_fileTransferActions = fileTransferActions;
}

void FileTransferManager::setFileTransferHandlerManager(FileTransferHandlerManager *fileTransferHandlerManager)
{
	m_fileTransferHandlerManager = fileTransferHandlerManager;
}

void FileTransferManager::setIconsManager(IconsManager *iconsManager)
{
	m_iconsManager = iconsManager;
}

void FileTransferManager::setInjectedFactory(InjectedFactory *injectedFactory)
{
	m_injectedFactory = injectedFactory;
}

void FileTransferManager::setKaduWindowService(KaduWindowService *kaduWindowService)
{
	m_kaduWindowService = kaduWindowService;
}

void FileTransferManager::setNotificationCallbackRepository(NotificationCallbackRepository *notificationCallbackRepository)
{
	m_notificationCallbackRepository = notificationCallbackRepository;
}

void FileTransferManager::setNotificationEventRepository(NotificationEventRepository *notificationEventRepository)
{
	m_notificationEventRepository = notificationEventRepository;
}

void FileTransferManager::setNotificationManager(NotificationManager *notificationManager)
{
	m_notificationManager = notificationManager;
}

void FileTransferManager::init()
{
	NewFileTransferNotification::registerEvents(m_notificationEventRepository, m_notificationCallbackRepository);
	m_configurationManager->registerStorableObject(this);
	triggerAllAccountsRegistered(m_accountManager);
}

void FileTransferManager::done()
{
	triggerAllAccountsUnregistered(m_accountManager);
	m_configurationManager->unregisterStorableObject(this);
	if (m_window)
		m_window.data()->deleteLater();
	NewFileTransferNotification::unregisterEvents(m_notificationEventRepository);
}

void FileTransferManager::addFileTransferService(Account account)
{
	auto protocol = account.protocolHandler();
	if (!protocol)
		return;

	auto service = protocol->fileTransferService();
	if (!service)
		return;

	connect(service, SIGNAL(incomingFileTransfer(FileTransfer)),
			this, SLOT(incomingFileTransfer(FileTransfer)));
}

void FileTransferManager::removeFileTransferService(Account account)
{
	auto protocol = account.protocolHandler();
	if (!protocol)
		return;

	auto service = protocol->fileTransferService();
	if (!service)
		return;

	disconnect(service, 0, this, 0);
}

FileTransfer FileTransferManager::loadStubFromStorage(const std::shared_ptr<StoragePoint> &storagePoint)
{
	return FileTransfer::loadStubFromStorage(storagePoint);
}

void FileTransferManager::accountRegistered(Account account)
{
	QMutexLocker locker(&mutex());

	addFileTransferService(account);
}

void FileTransferManager::accountUnregistered(Account account)
{
	QMutexLocker locker(&mutex());

	removeFileTransferService(account);
}

void FileTransferManager::cleanUp()
{
	QMutexLocker locker(&mutex());

	auto toRemove = QList<FileTransfer>{};

	for (auto &&fileTransfer : items())
	{
		if (FileTransferStatus::Finished == fileTransfer.transferStatus())
			toRemove.append(fileTransfer);
		if (FileTransferStatus::Rejected == fileTransfer.transferStatus() && FileTransferDirection::Incoming == fileTransfer.transferDirection())
			toRemove.append(fileTransfer);
	}

	for (auto &&fileTransfer : toRemove)
		removeItem(fileTransfer);
}

void FileTransferManager::acceptFileTransfer(FileTransfer transfer, QString localFileName)
{
	if (!m_fileTransferHandlerManager->ensureHandler(transfer))
		return;

	auto chat = ChatTypeContact::findChat(m_chatManager, transfer.peer(), ActionReturnNull);
	QWidget *parent = m_chatWidgetRepository->widgetForChat(chat);
	if (parent == nullptr)
		parent = m_kaduWindowService->kaduWindow();

	auto remoteFileName = transfer.remoteFileName();
	auto saveFileName = localFileName;

	while (true)
	{
		saveFileName = getSaveFileName(saveFileName, remoteFileName, parent);
		if (saveFileName.isEmpty())
			break;

		auto file = new QFile{saveFileName};
		if (!file->open(QFile::WriteOnly | QIODevice::Truncate))
		{
			MessageDialog::show(m_iconsManager->iconByPath(KaduIcon("dialog-warning")), tr("Kadu"), tr("Could not open file. Select another one."));
			saveFileName.clear();
			file->deleteLater();
			continue;
		}

		transfer.setLocalFileName(saveFileName);

		auto streamHandler = qobject_cast<StreamIncomingFileTransferHandler *>(transfer.handler());
		if (streamHandler)
			streamHandler->accept(file);

		auto urlHandler = qobject_cast<UrlIncomingFileTransferHandler *>(transfer.handler());
		if (urlHandler)
			urlHandler->save(file);

		transfer.setTransferStatus(FileTransferStatus::Transfer);
		showFileTransferWindow();
		break;
	}
}

void FileTransferManager::rejectFileTransfer(FileTransfer transfer)
{
	QMutexLocker locker(&mutex());

	m_fileTransferHandlerManager->ensureHandler(transfer);
	auto streamHandler = qobject_cast<StreamIncomingFileTransferHandler *>(transfer.handler());
	if (streamHandler)
		streamHandler->reject();
}

void FileTransferManager::sendFile(FileTransfer transfer, QString fileName)
{
	if (transfer.transferDirection() != FileTransferDirection::Outgoing)
		return;

	auto fileInfo = QFileInfo{fileName};
	transfer.setFileSize(fileInfo.size());
	transfer.setLocalFileName(fileName);
	transfer.setRemoteFileName(fileInfo.fileName());
	transfer.setTransferredSize(0);

	auto handler = qobject_cast<OutgoingFileTransferHandler *>(transfer.handler());

	if (handler)
	{
		auto file = new QFile{fileName};
		if (!file->open(QIODevice::ReadOnly))
		{
			transfer.setError(tr("Unable to open local file"));
			file->deleteLater();
			return;
		}

		handler->send(file);
	}
}

QString FileTransferManager::getSaveFileName(QString fileName, QString remoteFileName, QWidget *parent)
{
	auto haveFileName = !fileName.isEmpty();
	// auto resumeTransfer = haveFileName;

	while (true)
	{
		if (fileName.isEmpty())
			fileName = QFileDialog::getSaveFileName(parent, tr("Select file location"),
					m_configuration->deprecatedApi()->readEntry("Network", "LastDownloadDirectory") + remoteFileName,
							QString(), 0, QFileDialog::DontConfirmOverwrite);

		if (fileName.isEmpty())
			return fileName;

		m_configuration->deprecatedApi()->writeEntry("Network", "LastDownloadDirectory", QFileInfo(fileName).absolutePath() + '/');
		auto info = QFileInfo{fileName};

		if (!haveFileName && info.exists())
		{
			auto question = tr("File %1 already exists.").arg(fileName);
			switch (QMessageBox::question(parent, tr("Save file"), question, tr("Overwrite"), //tr("Resume"),
			                                 tr("Select another file"), 0, 2))
			{
				case 0:
					//resumeTransfer = false;
					break;

				case 1:
					//resumeTransfer = true;
					//break;

				//case 2:
					fileName = QString{};
					haveFileName = false;
					continue;
			}
		}

		if (info.exists() && !info.isWritable())
		{
			MessageDialog::show(m_iconsManager->iconByPath(KaduIcon("dialog-warning")), tr("Kadu"), tr("Could not open file. Select another one."));
			fileName.clear();
			continue;
		}

		return fileName;
	}
}

void FileTransferManager::showFileTransferWindow()
{
	QMutexLocker locker(&mutex());

	if (!m_window)
		m_window = m_injectedFactory->makeInjected<FileTransferWindow>();
	_activateWindow(m_configuration, m_window.data());
}

void FileTransferManager::incomingFileTransfer(FileTransfer fileTransfer)
{
	QMutexLocker locker(&mutex());
	addItem(fileTransfer);
	NewFileTransferNotification::notifyIncomingFileTransfer(m_injectedFactory, m_chatManager, m_notificationManager, fileTransfer);
}

void FileTransferManager::itemAboutToBeAdded(FileTransfer fileTransfer)
{
	emit fileTransferAboutToBeAdded(fileTransfer);
}

void FileTransferManager::itemAdded(FileTransfer fileTransfer)
{
	emit fileTransferAdded(fileTransfer);
	connect(fileTransfer, SIGNAL(updated()), this, SLOT(updateProgress()));
	updateProgress();
}

void FileTransferManager::itemAboutToBeRemoved(FileTransfer fileTransfer)
{
	emit fileTransferAboutToBeRemoved(fileTransfer);
}

void FileTransferManager::itemRemoved(FileTransfer fileTransfer)
{
	emit fileTransferRemoved(fileTransfer);
	disconnect(fileTransfer, SIGNAL(updated()), this, SLOT(updateProgress()));
	updateProgress();
}

void FileTransferManager::updateProgress()
{
	auto transferredSize = 0ul;
	auto totalSize = 0ul;

	for (auto &&fileTransfer : items())
		if (FileTransferStatus::Transfer == fileTransfer.transferStatus())
		{
			transferredSize += fileTransfer.transferredSize();
			totalSize += fileTransfer.fileSize();
		}

	if (totalSize == 0 || totalSize == transferredSize)
	{
		setTotalProgress(100);
		return;
	}

	setTotalProgress(100 * transferredSize / totalSize);
}

void FileTransferManager::setTotalProgress(int totalProgress)
{
	if (m_totalProgress == totalProgress)
		return;

	m_totalProgress = totalProgress;
	emit totalProgressChanged(totalProgress);
}

int FileTransferManager::totalProgress() const
{
	return m_totalProgress;
}

#include "moc_file-transfer-manager.cpp"
