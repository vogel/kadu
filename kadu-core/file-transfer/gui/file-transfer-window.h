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

#pragma once

#include "file-transfer/file-transfer.h"
#include "misc/memory.h"
#include "os/generic/desktop-aware-object.h"

#include <QtCore/QPointer>
#include <QtWidgets/QFrame>
#include <injeqt/injeqt.h>

class QScrollArea;
class QVBoxLayout;

class Configuration;
class FileTransferManager;
class FileTransferWidget;
class InjectedFactory;
class MainWindowRepository;

class FileTransferWindow : public QFrame, DesktopAwareObject
{
    Q_OBJECT

protected:
    virtual void keyPressEvent(QKeyEvent *e);

    void contentsChanged();

public:
    explicit FileTransferWindow(QWidget *parent = nullptr);
    virtual ~FileTransferWindow();

private:
    QPointer<Configuration> m_configuration;
    QPointer<FileTransferManager> m_fileTransferManager;
    QPointer<InjectedFactory> m_injectedFactory;
    QPointer<MainWindowRepository> m_mainWindowRepository;

    std::vector<FileTransferWidget *> m_widgets;

    owned_qptr<QScrollArea> m_scrollView;

    owned_qptr<QFrame> m_innerFrame;
    owned_qptr<QVBoxLayout> m_transfersLayout;

    void createGui();

private slots:
    INJEQT_SET void setConfiguration(Configuration *configuration);
    INJEQT_SET void setFileTransferManager(FileTransferManager *fileTransferManager);
    INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
    INJEQT_SET void setMainWindowRepository(MainWindowRepository *mainWindowRepository);
    INJEQT_INIT void init();
    INJEQT_DONE void done();

    void fileTransferAdded(FileTransfer fileTransfer);
    void fileTransferRemoved(FileTransfer fileTransfer);

    void clearClicked();
};
