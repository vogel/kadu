/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#ifndef JABBER_FILE_TRANSFER_HANDLER_H
#define JABBER_FILE_TRANSFER_HANDLER_H

#include <QtCore/QFile>

#include <jid.h>
#include <s5b.h>

#include "file-transfer/file-transfer-handler.h"

namespace XMPP
{
	class FileTransfer;
};

class JabberFileTransferHandlerPrivate;
class JabberFileTransferHandler : public FileTransferHandler
{
	Q_OBJECT

	Q_DECLARE_PRIVATE(JabberFileTransferHandler);
	JabberFileTransferHandlerPrivate * const d_ptr;

	XMPP::FileTransfer *JabberTransfer;
	XMPP::Jid PeerJid;

	bool InProgress;
	qlonglong BytesTransferred;
	QFile LocalFile;

	void connectJabberTransfer();
	void disconnectJabberTransfer();

protected:
	virtual void updateFileInfo();

private slots:
	void fileTransferAccepted();
	void fileTransferConnected();
	void fileTransferReadyRead(const QByteArray &a);
	void fileTransferBytesWritten(int);
	void fileTransferError(int);

	void s5b_proxyQuery();
	void s5b_proxyResult();
	void s5b_requesting();
	void s5b_accepted();
	void s5b_proxyConnect();
	void s5b_waitingForActivation();

	void trySend();
	void doFinish();
	void slotIncomingDataReady(const QByteArray &data);
	void slotTransferError(int errorCode);

public:
	JabberFileTransferHandler(FileTransfer fileTransfer);
	virtual ~JabberFileTransferHandler();

	void setJTransfer(XMPP::FileTransfer *jTransfer);

	virtual void send();
	virtual void stop();
	virtual void pause();
	virtual void restore();

	virtual bool accept(QFile &file);
	virtual void reject();

signals:
	void statusChanged();

};

#endif // JABBER_FILE_TRANSFER_HANDLER_H
