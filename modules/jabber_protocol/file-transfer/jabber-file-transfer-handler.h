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

#include "file-transfer/file-transfer-handler.h"
#include <xmpp.h>

class JabberProtocol;
namespace XMPP
{
	class FileTransfer;
};

class JabberFileTransferHandler : public FileTransferHandler
{
	Q_OBJECT
	
	XMPP::FileTransfer *JabberTransfer;
	bool InProgress;
	qlonglong BytesSent, Offset, Length;
	qlonglong BytesTransferred;
	qlonglong mBytesToTransfer;
	int Shift;
	int Complement;
	XMPP::Jid PeerJid;
	QString Description;
	QFile LocalFile;

protected:
	virtual void updateFileInfo();

private slots:
	// ft
	void ft_accepted();
	void ft_connected();
	void ft_readyRead(const QByteArray &);
	void ft_bytesWritten(int);
	void ft_error(int);
	void ft_error(int, int, const QString &);
	void trySend();
	void doFinish();
	void slotIncomingDataReady(const QByteArray &data);
	void slotTransferError(int errorCode);

public:
	enum { ErrReject, ErrTransfer, ErrFile };
	enum { Sending, Receiving };

	JabberFileTransferHandler(FileTransfer fileTransfer);
	virtual ~JabberFileTransferHandler();

	void setJTransfer(XMPP::FileTransfer *jTransfer);

	virtual void send();
	virtual void stop();
	virtual void pause();
	virtual void restore();

	virtual bool accept(const QFile &file);
	virtual void reject();

signals:
	void statusChanged();

};

#endif // JABBER_FILE_TRANSFER_HANDLER_H
