/*
 * %kadu copyright begin%
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
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

#include <QtCore/QFileInfo>

#include <xmpp/xmpp-im/xmpp_bytestream.h>
#include <filetransfer.h>

#include "resource/jabber-resource-pool.h"
#include "jabber-protocol.h"

#include "jabber-file-transfer-handler-private.h"
#include "jabber-file-transfer-handler.h"

JabberFileTransferHandler::JabberFileTransferHandler(::FileTransfer transfer) :
		FileTransferHandler(transfer), d_ptr(new JabberFileTransferHandlerPrivate(this)), JabberTransfer(0), InProgress(false)
{
}

JabberFileTransferHandler::~JabberFileTransferHandler()
{
}

void JabberFileTransferHandler::connectJabberTransfer()
{
	if (!JabberTransfer)
		return;

	connect(JabberTransfer, SIGNAL(accepted()), this, SLOT(fileTransferAccepted()));
	connect(JabberTransfer, SIGNAL(connected()), this, SLOT(fileTransferConnected()));
	connect(JabberTransfer, SIGNAL(readyRead(const QByteArray &)), this, SLOT(fileTransferReadyRead(const QByteArray &)));
	connect(JabberTransfer, SIGNAL(bytesWritten(int)), this, SLOT(fileTransferBytesWritten(int)));
	connect(JabberTransfer, SIGNAL(error(int)), this, SLOT(fileTransferError(int)));

	if (JabberTransfer->bsConnection())
	{
		Q_D(JabberFileTransferHandler);

		printf("bs connection is: %s\n", JabberTransfer->bsConnection()->metaObject()->className());

		connect(JabberTransfer->bsConnection(), SIGNAL(proxyQuery()), SLOT(s5b_proxyQuery()));
		connect(JabberTransfer->bsConnection(), SIGNAL(proxyResult(bool)), SLOT(s5b_proxyResult(bool)));
		connect(JabberTransfer->bsConnection(), SIGNAL(requesting()), SLOT(s5b_requesting()));
		connect(JabberTransfer->bsConnection(), SIGNAL(accepted()), SLOT(s5b_accepted()));
		connect(JabberTransfer->bsConnection(), SIGNAL(tryingHosts(const StreamHostList &)), d, SLOT(s5b_tryingHosts(const StreamHostList &)));
		connect(JabberTransfer->bsConnection(), SIGNAL(proxyConnect()), SLOT(s5b_proxyConnect()));
		connect(JabberTransfer->bsConnection(), SIGNAL(waitingForActivation()), SLOT(s5b_waitingForActivation()));
	}
}

void JabberFileTransferHandler::disconnectJabberTransfer()
{
	if (!JabberTransfer)
		return;

	disconnect(JabberTransfer, SIGNAL(accepted()), this, SLOT(fileTransferAccepted()));
	disconnect(JabberTransfer, SIGNAL(connected()), this, SLOT(fileTransferConnected()));
	disconnect(JabberTransfer, SIGNAL(readyRead(const QByteArray &)), this, SLOT(fileTransferReadyRead(const QByteArray &)));
	disconnect(JabberTransfer, SIGNAL(bytesWritten(int)), this, SLOT(fileTransferBytesWritten(int)));
	disconnect(JabberTransfer, SIGNAL(error(int)), this, SLOT(fileTransferError(int)));
}

void JabberFileTransferHandler::setJTransfer(XMPP::FileTransfer *jTransfer)
{
	disconnectJabberTransfer();
	JabberTransfer = jTransfer;
	connectJabberTransfer();
}

void JabberFileTransferHandler::updateFileInfo()
{
	if (JabberTransfer)
	{
		transfer().setFileSize(LocalFile.size());
		transfer().setTransferredSize(BytesTransferred);
	}
	else
	{
		transfer().setFileSize(0);
		transfer().setTransferredSize(0);
	}

	emit statusChanged();
}

void JabberFileTransferHandler::send()
{
	if (TypeSend != transfer().transferType()) // maybe assert here?
		return;

	if (InProgress) // already sending/receiving
		return;

	transfer().setRemoteFileName(transfer().localFileName());

	QFileInfo fileInfo(transfer().localFileName());
	transfer().setFileSize(fileInfo.size());

	Account account = transfer().peer().contactAccount();
	if (account.isNull() || transfer().localFileName().isEmpty())
	{
		transfer().setTransferStatus(StatusNotConnected);
		return; // TODO: notify
	}

	JabberProtocol *jabberProtocol = dynamic_cast<JabberProtocol *>(account.protocolHandler());
	if (!jabberProtocol)
	{
		transfer().setTransferStatus(StatusNotConnected);
		return;
	}

	if (!jabberProtocol->jabberContactDetails(transfer().peer()))
	{
		transfer().setTransferStatus(StatusNotConnected);
		return;
	}

	XMPP::Jid proxy;
	JabberAccountDetails *jabberAccountDetails = dynamic_cast<JabberAccountDetails *>(account.details());
	if (0 != jabberAccountDetails)
		proxy = jabberAccountDetails->dataTransferProxy();

	QString jid = transfer().peer().id();
	// sendFile needs jid with resource so take best from ResourcePool
	PeerJid = XMPP::Jid(jid).withResource(jabberProtocol->resourcePool()->bestResource(jid).name());

	if (!JabberTransfer)
	{
		JabberTransfer = jabberProtocol->xmppClient()->fileTransferManager()->createTransfer();
		connectJabberTransfer();
	}

// 	if (proxy.isValid())
// 		JabberTransfer->setProxy(proxy);

	transfer().setTransferStatus(StatusWaitingForConnection);
	InProgress = true;

	JabberTransfer->sendFile(PeerJid, transfer().localFileName(), transfer().fileSize(), QString());
}

void JabberFileTransferHandler::stop()
{
	if (JabberTransfer)
	{
		JabberTransfer->close();
		JabberTransfer->deleteLater();
		JabberTransfer = 0;
		transfer().setTransferStatus(StatusNotConnected);
	}
}

void JabberFileTransferHandler::pause()
{
	stop();
}

void JabberFileTransferHandler::restore()
{
	if (TypeSend == transfer().transferType())
		send();
}

/**
 * @todo do not pass opened file to this method
 */
bool JabberFileTransferHandler::accept(QFile &file)
{
	// this suxx, I know
	file.close();
	LocalFile.setFileName(file.fileName());

	if (JabberTransfer->rangeSupported())
	{
		if (!LocalFile.open(QIODevice::Append | QIODevice::WriteOnly))
			return false;
	}
	else
	{
		// we have to close file and reopen it
		if (!LocalFile.open(QIODevice::Truncate | QIODevice::WriteOnly))
			return false;
	}

	BytesTransferred = file.size();

	transfer().accept(file);
	transfer().setTransferStatus(StatusTransfer);
	transfer().setTransferredSize(BytesTransferred);

	JabberTransfer->accept(BytesTransferred);

	return true;
}

void JabberFileTransferHandler::reject()
{
	if (JabberTransfer)
		JabberTransfer->close();

	deleteLater();
}

using namespace XMPP;

void JabberFileTransferHandler::fileTransferAccepted()
{
	transfer().setTransferStatus(StatusTransfer);

	if (JabberTransfer->bsConnection())
	{
		printf("[%p] ACCEPTED bs connection is: %s\n", this, JabberTransfer->bsConnection()->metaObject()->className());

		S5BConnection *s5b = dynamic_cast<S5BConnection*>(JabberTransfer->bsConnection());
		if (s5b)
		{
			Q_D(JabberFileTransferHandler);

			printf("[%p] s5b connection\n", this);

			printf("[%p] c1: %d\n", this, connect(s5b, SIGNAL(proxyQuery()), SLOT(s5b_proxyQuery())));
			printf("[%p] c2: %d\n", this, connect(s5b, SIGNAL(proxyResult(bool)), SLOT(s5b_proxyResult())));
			printf("[%p] c3: %d\n", this, connect(s5b, SIGNAL(requesting()), SLOT(s5b_requesting())));
			printf("[%p] c4: %d\n", this, connect(s5b, SIGNAL(accepted()), SLOT(s5b_accepted())));
			printf("[%p] c5: %d\n", this, connect(s5b, SIGNAL(tryingHosts(const StreamHostList &)), d, SLOT(s5b_tryingHosts(const StreamHostList &))));
			printf("[%p] c6: %d\n", this, connect(s5b, SIGNAL(proxyConnect()), SLOT(s5b_proxyConnect())));
			printf("[%p] c7: %d\n", this, connect(s5b, SIGNAL(waitingForActivation()), SLOT(s5b_waitingForActivation())));
		}
	}
}

void JabberFileTransferHandler::s5b_proxyQuery()
{
	printf("[%p] s5b_proxyQuery\n", this);
}

void JabberFileTransferHandler::s5b_proxyResult()
{
	printf("[%p] s5b_proxyResult\n", this);
}

void JabberFileTransferHandler::s5b_requesting()
{
	printf("[%p] s5b_requesting\n", this);
}

void JabberFileTransferHandler::s5b_accepted()
{
	printf("[%p] s5b_accepted\n", this);
}

void JabberFileTransferHandler::s5b_proxyConnect()
{
	printf("[%p] s5b_proxyConnect\n", this);
}

void JabberFileTransferHandler::s5b_waitingForActivation()
{
	printf("[%p] s5b_waitingForActivation\n", this);
}


void JabberFileTransferHandler::fileTransferConnected()
{
/*	d->sent = d->offset;

	if(d->sending) {
		// open the file, and set the correct offset
		bool ok = false;
		if(d->f.open(QIODevice::ReadOnly)) {
			if(d->offset == 0) {
				ok = true;
			}
			else {
				if(d->f.at(d->offset))
					ok = true;
			}
		}
		if(!ok) {
			delete d->ft;
			d->ft = 0;
			error(ErrFile, 0, d->f.errorString());
			return;
		}

		if(d->sent == d->fileSize)
			QTimer::singleShot(0, this, SLOT(doFinish()));
		else
			QTimer::singleShot(0, this, SLOT(trySend()));
	}
	else {
		// open the file, truncating if offset is zero, otherwise set the correct offset
		QIODevice::OpenMode m = QIODevice::ReadWrite;
		if(d->offset == 0)
			m |= QIODevice::Truncate;
		bool ok = false;
		if(d->f.open(m)) {
			if(d->offset == 0) {
				ok = true;
			}
			else {
				if(d->f.at(d->offset))
					ok = true;
			}
		}
		if(!ok) {
			delete d->ft;
			d->ft = 0;
			error(ErrFile, 0, d->f.errorString());
			return;
		}

		d->activeFile = d->f.name();
		active_file_add(d->activeFile);

		// done already?  this means a file size of zero
		if(d->sent == d->fileSize)
			QTimer::singleShot(0, this, SLOT(doFinish()));
	}

	connected();*/
}


void JabberFileTransferHandler::fileTransferReadyRead(const QByteArray &a)
{
	Q_UNUSED(a)

/*	if(!d->sending) {
		//printf("%d bytes read\n", a.size());
		int r = d->f.writeBlock(a.data(), a.size());
		if(r < 0) {
			d->f.close();
			delete d->ft;
			d->ft = 0;
			error(ErrFile, 0, d->f.errorString());
			return;
		}
		d->sent += a.size();
		doFinish();
	}
*/
}

void JabberFileTransferHandler::fileTransferBytesWritten(int x)
{
	Q_UNUSED(x)

/*
	if(d->sending) {
		//printf("%d bytes written\n", x);
		d->sent += x;
		if(d->sent == d->fileSize) {
			d->f.close();
			delete d->ft;
			d->ft = 0;
		}
		else
			QTimer::singleShot(0, this, SLOT(trySend()));
		progress(calcProgressStep(d->sent, JabberTransfer->bsConnectionomplement, d->shift), d->sent);
	}
*/
}

void JabberFileTransferHandler::fileTransferError(int error)
{
	if (LocalFile.isOpen())
		LocalFile.close();
	JabberTransfer->deleteLater();
	JabberTransfer = 0;

	switch (error)
	{
		case XMPP::FileTransfer::ErrReject:
			transfer().setTransferStatus(StatusRejected);
			break;
		case XMPP::FileTransfer::ErrNeg:
		case XMPP::FileTransfer::ErrConnect:
		case XMPP::FileTransfer::ErrStream:
		default:
			transfer().setTransferStatus(StatusNotConnected);
			break;
	}
}

void JabberFileTransferHandler::trySend()
{
/*
	// Since trySend comes from singleShot which is an "uncancelable"
	//   action, we should protect that d->ft is valid, for good measure
	if(!d->ft)
		return;

	// When FileTransfer emits error, you are not allowed to call
	//   dataSizeNeeded() afterwards.  Simetime ago, we changed to using
	//   QueuedConnection for error() signal delivery (see mapSignals()).
	//   This made it possible to call dataSizeNeeded by accident between
	//   the error() signal emit and the ft_error() slot invocation.  To
	//   work around this problem, we'll check to see if the FileTransfer
	//   is internally active by checking if s5bConnection() is null.
	//   FIXME: this probably breaks other file transfer methods, whenever
	//   we get those.  Probably we need a real fix in Iris..
	if(!d->ft->s5bConnection())
		return;

	int blockSize = d->ft->dataSizeNeeded();
	QByteArray a(blockSize, 0);
	int r = 0;
	if(blockSize > 0)
		r = d->f.read(a.data(), a.size());
	if(r < 0) {
		d->f.close();
		delete d->ft;
		d->ft = 0;
		error(ErrFile, 0, d->f.errorString());
		return;
	}
	if(r < (int)a.size())
		a.resize(r);
	d->ft->writeFileData(a);
*/
}

void JabberFileTransferHandler::slotIncomingDataReady ( const QByteArray &data )
{
	//if(!d->sending) {
		//printf("%d bytes read\n", a.size());
		int r = LocalFile.write(data.data(), data.size());
		if(r < 0) {
			LocalFile.close();
			delete JabberTransfer;
			JabberTransfer = 0;
			transfer().setTransferError(ErrorUnableToOpenFile);
			return;
		}
		BytesTransferred += data.size();
		doFinish();
	//}
	emit statusChanged();
}

void JabberFileTransferHandler::doFinish()
{
	if (BytesTransferred == JabberTransfer->length())
	{
		LocalFile.close();

		delete JabberTransfer;
		JabberTransfer = 0;

		transfer().setTransferStatus(StatusFinished);
	}
}

