/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "accounts/accounts-aware-object.h"
#include "exports.h"
#include "file-transfer/file-transfer.h"
#include "storage/manager.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QUuid>
#include <injeqt/injeqt.h>

class AccountManager;
class ChatManager;
class ChatStorage;
class ChatWidgetRepository;
class ConfigurationApi;
class ConfigurationManager;
class Configuration;
class FileTransferActions;
class FileTransferHandlerManager;
class FileTransferNotificationService;
class FileTransferStorage;
class FileTransferWindow;
class IconsManager;
class InjectedFactory;
class KaduWindowService;

class KADUAPI FileTransferManager : public Manager<FileTransfer>, AccountsAwareObject
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit FileTransferManager(QObject *parent = nullptr);
    virtual ~FileTransferManager();

    virtual QString storageNodeName() override
    {
        return QStringLiteral("FileTransfersNew");
    }
    virtual QString storageNodeItemName() override
    {
        return QStringLiteral("FileTransfer");
    }

    int totalProgress() const;

    void acceptFileTransfer(FileTransfer transfer, QString localFileName);
    void rejectFileTransfer(FileTransfer transfer);
    void sendFile(FileTransfer transfer, QString fileName);
    QString getSaveFileName(QString localFileName, QString remoteFileName, QWidget *parent = nullptr);
    void showFileTransferWindow();
    void cleanUp();

signals:
    void fileTransferAboutToBeAdded(FileTransfer fileTransfer);
    void fileTransferAdded(FileTransfer fileTransfer);
    void fileTransferAboutToBeRemoved(FileTransfer fileTransfer);
    void fileTransferRemoved(FileTransfer fileTransfer);
    void totalProgressChanged(int totalProgress);

protected:
    virtual FileTransfer loadStubFromStorage(const std::shared_ptr<StoragePoint> &storagePoint) override;

    virtual void accountAdded(Account account) override;

    virtual void itemAboutToBeAdded(FileTransfer fileTransfer) override;
    virtual void itemAdded(FileTransfer fileTransfer) override;
    virtual void itemAboutToBeRemoved(FileTransfer fileTransfer) override;
    virtual void itemRemoved(FileTransfer fileTransfer) override;

private:
    QPointer<AccountManager> m_accountManager;
    QPointer<ChatManager> m_chatManager;
    QPointer<ChatStorage> m_chatStorage;
    QPointer<ChatWidgetRepository> m_chatWidgetRepository;
    QPointer<ConfigurationManager> m_configurationManager;
    QPointer<Configuration> m_configuration;
    QPointer<FileTransferActions> m_fileTransferActions;
    QPointer<FileTransferHandlerManager> m_fileTransferHandlerManager;
    QPointer<FileTransferNotificationService> m_fileTransferNotificationService;
    QPointer<FileTransferStorage> m_fileTransferStorage;
    QPointer<FileTransferWindow> m_window;
    QPointer<IconsManager> m_iconsManager;
    QPointer<InjectedFactory> m_injectedFactory;
    QPointer<KaduWindowService> m_kaduWindowService;
    int m_totalProgress;

    void setTotalProgress(int totalProgress);

private slots:
    INJEQT_SET void setAccountManager(AccountManager *accountManager);
    INJEQT_SET void setChatManager(ChatManager *chatManager);
    INJEQT_SET void setChatStorage(ChatStorage *chatStorage);
    INJEQT_SET void setChatWidgetRepository(ChatWidgetRepository *chatWidgetRepository);
    INJEQT_SET void setConfigurationManager(ConfigurationManager *configurationManager);
    INJEQT_SET void setConfiguration(Configuration *configuration);
    INJEQT_SET void setFileTransferActions(FileTransferActions *fileTransferActions);
    INJEQT_SET void setFileTransferHandlerManager(FileTransferHandlerManager *fileTransferHandlerManager);
    INJEQT_SET void
    setFileTransferNotificationService(FileTransferNotificationService *fileTransferNotificationService);
    INJEQT_SET void setFileTransferStorage(FileTransferStorage *fileTransferStorage);
    INJEQT_SET void setIconsManager(IconsManager *iconsManager);
    INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
    INJEQT_SET void setKaduWindowService(KaduWindowService *kaduWindowService);
    INJEQT_INIT void init();
    INJEQT_DONE void done();

    void incomingFileTransfer(FileTransfer fileTransfer);
    void updateProgress();

    void protocolHandlerChanged(Account account);
};
