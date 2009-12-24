/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef DCC_SOCKET_NOTIFIERS_H
#define DCC_SOCKET_NOTIFIERS_H

#include <libgadu.h>

#include "buddies/buddy.h"

#include "dcc/dcc-manager.h"
#include "socket-notifiers/gadu-socket-notifiers.h"
#include "gadu-protocol.h"

#if __GNUC__ >= 4
	#define GADU_LOCAL __attribute__ ((visibility("hidden")))
#else
	#define GADU_LOCAL
#endif /* __GNUC__ >= 4 */

#ifndef _MSC_VER
	#pragma GCC visibility push(hidden)
#endif /* _MSC_VER */

class QFile;

class GaduFileTransferHandler;

class GADU_LOCAL DccSocketNotifiers : public GaduSocketNotifiers
{
	Q_OBJECT

	GaduProtocol *Protocol;

	friend class DccManager;
	DccManager *Manager;

	GaduFileTransferHandler *FileTransferHandler;

	DccVersion Version;
	struct gg_dcc *Socket;
	struct gg_dcc7 *Socket7;
	int *DccCheckField;

	void accepted();
	void rejected();
	void finished(bool ok);

	void handleEventDccError(struct gg_event *e);
	void handleEventDccDone(struct gg_event *e);
	void handleEventDccClientAccept(struct gg_event *e);
	void handleEventDccCallback(struct gg_event *e);
	void handleEventDccNeedFileInfo(struct gg_event *e);
	void handleEventDccNeedFileAck(struct gg_event *e);
	void handleEventDccNeedVoiceAck(struct gg_event *e);
	void handleEventDccVoiceData(struct gg_event *e);

	void handleEventDcc7Accept(struct gg_event *e);
	void handleEventDcc7Reject(struct gg_event *e);
	void handleEventDcc7Connected(struct gg_event *e);
	void handleEventDcc7Error(struct gg_event *e);
	void handleEventDcc7Done(struct gg_event *e);
	void handleEventDcc7Pending(struct gg_event *e);

private slots:
	void dcc7Accepted(struct gg_dcc7 *);
	void dcc7Rejected(struct gg_dcc7 *);

protected:
	DccManager * manager() { return Manager; }

	virtual bool checkRead();
	virtual bool checkWrite();
	virtual void socketEvent();
	virtual int timeout();
	virtual bool handleSoftTimeout();
	virtual void connectionTimeout();

public:
	DccSocketNotifiers(GaduProtocol *protocol, DccManager *manager) :
			GaduSocketNotifiers(manager), Protocol(protocol),
			Manager(manager), FileTransferHandler(0), DccCheckField(0) {}

	void watchFor(struct gg_dcc *socket);
	void watchFor(struct gg_dcc7 *socket);

	UinType peerUin();

	unsigned long fileSize();
	unsigned long transferredFileSize();
	QString remoteFileName();

	void setGaduFileTransferHandler(GaduFileTransferHandler *fileTransferHandler);
	bool acceptFileTransfer(const QFile &file);
	void rejectFileTransfer();

signals:
	void done(bool ok);

	void incomingConnection(struct gg_dcc *incomingConnection);
	void callbackReceived(DccSocketNotifiers *);

};

#ifndef _MSC_VER
	#pragma GCC visibility pop
#endif /* _MSC_VER */

#endif // DCC_SOCKET_NOTIFIERS_H
