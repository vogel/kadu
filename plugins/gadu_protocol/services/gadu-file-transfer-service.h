/*
 * %kadu copyright begin%
 * Copyright 2011, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <injeqt/injeqt.h>
#include <libgadu.h>

#include "protocols/protocol.h"
#include "protocols/services/file-transfer-service.h"

class FileTransferStorage;
class GaduFileTransferHandler;
class GaduIMTokenService;
class GaduProtocol;
class Myself;

class GaduFileTransferService : public FileTransferService
{
    Q_OBJECT

public:
    explicit GaduFileTransferService(GaduProtocol *protocol);
    virtual ~GaduFileTransferService();

    void setGaduIMTokenService(GaduIMTokenService *imTokenService);

    virtual FileTransferHandler *createFileTransferHandler(FileTransfer fileTransfer) override;
    virtual FileTransferCanSendResult canSend(Contact contact) override;

    void fileTransferReceived(Contact peer, QString downloadId, QString fileName);

private:
    QPointer<FileTransferStorage> m_fileTransferStorage;
    QPointer<GaduIMTokenService> m_imTokenService;
    QPointer<Myself> m_myself;

    GaduProtocol *Protocol;

private slots:
    INJEQT_SET void setFileTransferStorage(FileTransferStorage *fileTransferStorage);
    INJEQT_SET void setMyself(Myself *myself);
};
