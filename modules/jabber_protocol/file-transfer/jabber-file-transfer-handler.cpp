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
#include <QtGui/QMessageBox>

#include "accounts/account.h"
#include "buddies/buddy-manager.h"
#include "contacts/contact-manager.h"
#include "debug.h"
#include "../jabber-account-details.h"
#include "jabber-contact-details.h"
#include "../jabber-protocol.h"

#include "resource/jabber-resource-pool.h"
#include "libiris/include/iris/filetransfer.h"

#include "jabber-file-transfer-handler.h"

//typedef Q_UINT64 LARGE_TYPE;
typedef long long LARGE_TYPE;

#define CSMAX (sizeof(LARGE_TYPE)*8)
#define CSMIN 16
static int calcShift(qlonglong big)
{
	LARGE_TYPE val = 1;
	val <<= CSMAX - 1;
	for(int n = CSMAX - CSMIN; n > 0; --n) {
		if(big & val)
			return n;
		val >>= 1;
	}
	return 0;
}

static int calcComplement(qlonglong big, int shift)
{
	int block = 1 << shift;
	qlonglong rem = big % block;
	if(rem == 0)
		return 0;
	else
		return (block - (int)rem);
}

/*static int calcTotalSteps(qlonglong big, int shift)
{
	if(big < 1)
		return 0;
	return ((big - 1) >> shift) + 1;
}

static int calcProgressStep(qlonglong big, int complement, int shift)
{
	return ((big + complement) >> shift);
}

static QStringList *activeFiles = 0;

static void active_file_add(const QString &s)
{
	if(!activeFiles)
		activeFiles = new QStringList;
	activeFiles->append(s);
	//printf("added: [%s]\n", s.latin1());
}

static void active_file_remove(const QString &s)
{
	if(!activeFiles)
		return;
	activeFiles->removeAt(activeFiles->indexOf(s));
	//printf("removed: [%s]\n", s.latin1());
}

static bool active_file_check(const QString &s)
{
	if(!activeFiles)
		return false;
	return activeFiles->contains(s);
}

static QString clean_filename(const QString &s)
{
//#ifdef Q_OS_WIN
	QString badchars = "\\/|?*:\"<>";
	QString str;
	for(int n = 0; n < s.length(); ++n) {
		bool found = false;
		for(int b = 0; b < badchars.length(); ++b) {
			if(s.at(n) == badchars.at(b)) {
				found = true;
				break;
			}
		}
		if(!found)
			str += s;
	}
	if(str.isEmpty())
		str = "unnamed";
	return str;
//#else
//	return s;
//#endif
}*/


JabberFileTransferHandler::JabberFileTransferHandler(FileTransfer transfer) :
		FileTransferHandler(transfer), InProgress(false)
{
}

JabberFileTransferHandler::~JabberFileTransferHandler()
{
}

void JabberFileTransferHandler::setJTransfer(XMPP::FileTransfer* jTransfer)
{
	JabberTransfer = jTransfer;
}

void JabberFileTransferHandler::updateFileInfo()
{
// 	if (SocketNotifiers)
// 	{
		transfer().setFileSize(LocalFile.size());
		transfer().setTransferredSize(BytesSent);
// 	}
// 	else
// 	{
// 		setFileSize(0);
// 		setTransferredSize(0);
// 	}

// 	emit statusChanged();
}

void JabberFileTransferHandler::send()
{
	if (TypeSend != transfer().transferType()) // maybe assert here?
		return;

	if (InProgress) // already sending/receiving
		return;

	transfer().setRemoteFileName(transfer().localFileName());

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

	Shift = calcShift(transfer().fileSize());
	Complement = calcComplement(transfer().fileSize(), Shift);

	QString jid = transfer().peer().id();
	// sendFile needs jid with resource so take best from ResourcePool
	PeerJid = XMPP::Jid(jid).withResource(jabberProtocol->resourcePool()->bestResource(jid).name());

	JabberTransfer = jabberProtocol->client()->fileTransferManager()->createTransfer();
	if(proxy.isValid())
		JabberTransfer->setProxy(proxy);

	connect(JabberTransfer, SIGNAL(accepted()), SLOT(ft_accepted()));
	connect(JabberTransfer, SIGNAL(connected()), SLOT(ft_connected()));
	connect(JabberTransfer, SIGNAL(readyRead(const QByteArray &)), SLOT(ft_readyRead(const QByteArray &)));
	connect(JabberTransfer, SIGNAL(bytesWritten(int)), SLOT(ft_bytesWritten(int)));
	connect(JabberTransfer, SIGNAL(error(int)), SLOT(ft_error(int)),Qt::QueuedConnection);

	Description = "I iz in ur file transfer, steelin ur bytes";

	transfer().setTransferStatus(StatusWaitingForConnection);
	InProgress = true;
	JabberTransfer->sendFile(PeerJid, transfer().localFileName(), transfer().fileSize(), Description);
}

void JabberFileTransferHandler::stop()
{
// 	if (SocketNotifiers)
// 	{
// 		delete SocketNotifiers;
// 		SocketNotifiers = 0;
// 		changeFileTransferStatus(XMPP::FileTransfer::StatusNotConnected);
// 	}
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

bool JabberFileTransferHandler::accept(const QFile &file)
{
	transfer().accept(file);

	LocalFile.setFileName(file.fileName());// = QFile(QString());
	Length = JabberTransfer->fileSize();
	bool couldOpen = false;
	qlonglong offset = 0;
	/*qlonglong length = 0;*/

	transfer().setRemoteFileName(file.fileName());
	transfer().setFileSize(JabberTransfer->fileSize());
	transfer().setTransferredSize(BytesSent);

	transfer().setTransferStatus(StatusTransfer);

	BytesSent = 0;
	mBytesToTransfer = file.size();

	/*if ( mXMPPTransfer->rangeSupported () && mLocalFile.exists () )
	{
		KGuiItem resumeButton ( i18n ( "&Resume" ) );
		KGuiItem overwriteButton ( i18n ( "Over&write" ) );

		switch ( KMessageDialog::questionYesNoCancel ( Kopete::UI::Global::mainWidget (),
													i18n ( "The file %1 already exists, do you want to resume or overwrite it?", fileName ),
													i18n ( "File Exists: %1", fileName ),
													resumeButton, overwriteButton ) )
		{
			case KMessageDialog::Yes:		// resume
										couldOpen = mLocalFile.open ( QIODevice::ReadWrite );
										if ( couldOpen )
										{
											offset = mLocalFile.size ();
											length = mXMPPTransfer->fileSize () - offset;
											mBytesTransferred = offset;
											mBytesToTransfer = length;
											mLocalFile.seek ( mLocalFile.size () );
										}
										break;

			case KMessageDialog::No:		// overwrite
										couldOpen = mLocalFile.open ( QIODevice::WriteOnly );
										break;

			default:					// cancel
										deleteLater ();
										return;
		}
	}
	else
	{*/
		// overwrite by default
		couldOpen = LocalFile.open ( QIODevice::WriteOnly );
	//}

	if ( !couldOpen )
	{
		//transfer->slotError ( KIO::ERR_COULD_NOT_WRITE, fileName );

		//deleteLater ();
	}
	else
	{
		connect ( JabberTransfer, SIGNAL ( readyRead ( const QByteArray& ) ), this, SLOT ( slotIncomingDataReady ( const QByteArray & ) ) );
		connect ( JabberTransfer, SIGNAL ( error ( int ) ), this, SLOT ( slotTransferError ( int ) ) );
		JabberTransfer->accept(offset);
	}

	return true;

//	return SocketNotifiers->acceptFileTransfer(file);
}

void JabberFileTransferHandler::reject()
{
//	if (SocketNotifiers)
//		SocketNotifiers->rejectFileTransfer();

	deleteLater();
}

void JabberFileTransferHandler::ft_accepted()
{
	Offset = JabberTransfer->offset();
	Length = JabberTransfer->length();

kdebug("send file: accepted!");
/*
	d->c = d->ft->s5bConnection();
	connect(d->c, SIGNAL(proxyQuery()), SLOT(s5b_proxyQuery()));
	connect(d->c, SIGNAL(proxyResult(bool)), SLOT(s5b_proxyResult(bool)));
	connect(d->c, SIGNAL(requesting()), SLOT(s5b_requesting()));
	connect(d->c, SIGNAL(accepted()), SLOT(s5b_accepted()));
	connect(d->c, SIGNAL(tryingHosts(const StreamHostList &)), SLOT(s5b_tryingHosts(const StreamHostList &)));
	connect(d->c, SIGNAL(proxyConnect()), SLOT(s5b_proxyConnect()));
	connect(d->c, SfIGNAL(waitingForActivation()), SLOT(s5b_waitingForActivation()));

	if(d->sending)
		accepted();
	else
		statusMessage(QString());*/
}

void JabberFileTransferHandler::ft_connected()
{

kdebug("send file: connected!");
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


void JabberFileTransferHandler::ft_readyRead(const QByteArray &a)
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

void JabberFileTransferHandler::ft_bytesWritten(int x)
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
		progress(calcProgressStep(d->sent, d->complement, d->shift), d->sent);
	}
*/
}

void JabberFileTransferHandler::ft_error(int x)
{

	if(LocalFile.isOpen())
		LocalFile.close();
	delete JabberTransfer;
	JabberTransfer = 0;

	if(x == XMPP::FileTransfer::ErrReject)
		ft_error(ErrReject, x, QString());
	else if(x == XMPP::FileTransfer::ErrNeg)
		ft_error(ErrTransfer, x, tr("Unable to negotiate transfer."));
	else if(x == XMPP::FileTransfer::ErrConnect)
		ft_error(ErrTransfer, x, tr("Unable to connect to peer for data transfer."));
	else if(x == XMPP::FileTransfer::ErrProxy)
		ft_error(ErrTransfer, x, tr("Unable to connect to proxy for data transfer."));
	else if(x == XMPP::FileTransfer::ErrStream)
		ft_error(ErrTransfer, x, tr("Lost connection / Cancelled."));

}

void JabberFileTransferHandler::ft_error(int x, int fx, const QString &)
{
//	d->t.stop();
//	busy->stop();

//	delete d->ft;
//	d->ft = 0;

//	closeDialogs(this);

//	if(d->sending) {
// 		unblockWidgets();
// 		pb_stop->setText(tr("&Close"));
// 		lb_status->setText(tr("Ready"));
// 	}

	QString str;
	if(x == ErrReject)
		str = tr("File was rejected by remote user.");
	else if(x == ErrTransfer) {
		if(fx == XMPP::FileTransfer::ErrNeg)
			str = tr(
				"Unable to negotiate transfer.\n\n"
				"This can happen if the contact did not understand our request, or if the\n"
				"contact is offline."
				);
		else if(fx == XMPP::FileTransfer::ErrConnect)
			str = tr(
				"Unable to connect to peer for data transfer.\n\n"
				"Ensure that your Data Transfer settings are proper.  If you are behind\n"
				"a NAT router or firewall then you'll need to open the proper TCP port\n"
				"or specify a Data Transfer Proxy in your account settings."
				);
		else if(fx == XMPP::FileTransfer::ErrProxy)
			str = tr(
				"Failure to either connect to, or activate, the Data Transfer Proxy.\n\n"
				"This means that the Proxy service is either not functioning or it is\n"
				"unreachable.  If you are behind a firewall, then you'll need to ensure\n"
				"that outgoing TCP connections are allowed."
				);
	}
	else
		str = tr("File I/O error");
	QMessageBox::information(0, tr("Error"), str);

	//if(!d->sending || x == ErrReject)
	//	close();
}

void JabberFileTransferHandler::slotTransferError ( int errorCode )
{

	switch ( errorCode )
	{
		case XMPP::FileTransfer::ErrReject:
			// user rejected the transfer request
			transfer().setTransferStatus(StatusRejected);
			break;

		case XMPP::FileTransfer::ErrNeg:
			// unable to negotiate a suitable connection for the file transfer with the user
			transfer().setTransferStatus(StatusNotConnected);
			break;

		case XMPP::FileTransfer::ErrConnect:
			// could not connect to the user
			transfer().setTransferStatus(StatusNotConnected);
			break;

		case XMPP::FileTransfer::ErrStream:
			// data stream was disrupted, probably cancelled
			transfer().setTransferStatus(StatusNotConnected);
			break;

		default:
			// unknown error
			transfer().setTransferStatus(StatusNotConnected);
			break;
	}

//	deleteLater ();

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
	kdebug("Incoming data ...\n");
	//if(!d->sending) {
		//printf("%d bytes read\n", a.size());
		int r = LocalFile.write(data.data(), data.size());
		if(r < 0) {
			LocalFile.close();
			delete JabberTransfer;
			JabberTransfer = 0;
			ft_error(ErrFile, 0, LocalFile.errorString());
			return;
		}
		BytesSent += data.size();
		doFinish();
	//}
	emit statusChanged();
}

void JabberFileTransferHandler::doFinish()
{
	if(BytesSent == Length) {
		LocalFile.close();
		kdebug("Transfer finished... close file.\n");
		delete JabberTransfer;
		JabberTransfer = 0;
		transfer().setTransferStatus(StatusFinished);
	}
}

