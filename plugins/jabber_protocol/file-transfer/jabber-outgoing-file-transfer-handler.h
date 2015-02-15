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

#include <QtCore/QPointer>

#include <jid.h>
#include <s5b.h>

#include "file-transfer/outgoing-file-transfer-handler.h"

namespace XMPP
{
	class FileTransfer;
};

class JabberOutgoingFileTransferHandler : public OutgoingFileTransferHandler
{
	Q_OBJECT

	// a workaround to Qt's MOC not doing really well when mixing namespaces
	typedef XMPP::StreamHostList StreamHostList;

	XMPP::FileTransfer *JabberTransfer;
	XMPP::Jid PeerJid;

	bool InProgress;
	qlonglong BytesTransferred;
	QPointer<QIODevice> Source;

	void connectJabberTransfer();
	void disconnectJabberTransfer();

	FileTransferStatus errorToStatus(int error);
	void cleanup(FileTransferStatus status);

protected:
	virtual void updateFileInfo();

private slots:
	void fileTransferAccepted();
	void fileTransferConnected();
	void fileTransferBytesWritten(int);
	void fileTransferError(int);

public:
	explicit JabberOutgoingFileTransferHandler(FileTransfer fileTransfer);
	virtual ~JabberOutgoingFileTransferHandler();

	void setJTransfer(XMPP::FileTransfer *jTransfer);

	virtual void send(QIODevice *source);
	virtual void stop();

signals:
	void statusChanged();

};
