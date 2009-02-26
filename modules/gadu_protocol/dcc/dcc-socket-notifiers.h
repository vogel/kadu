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

#include "contacts/contact.h"

#include "dcc/dcc-manager.h"
#include "socket-notifiers/gadu-socket-notifiers.h"
#include "gadu-protocol.h"

#ifndef _MSC_VER
#pragma GCC visibility push(hidden)
#endif

class GaduFileTransfer;

class DccSocketNotifiers : public GaduSocketNotifiers
{
	Q_OBJECT

	GaduProtocol *Protocol;
	DccManager *Manager;
	GaduFileTransfer *FileTransfer;

	DccVersion Version;
	struct gg_dcc *Socket;
	struct gg_dcc7 *Socket7;
	int *DccCheckField;

	void finished(bool ok);

	void handleEventDccError(struct gg_event *e);
	void handleEventDccDone(struct gg_event *e);
	void handleEventDccClientAccept(struct gg_event *e);
	void handleEventDccCallback(struct gg_event *e);
	void handleEventDccNeedFileInfo(struct gg_event *e);
	void handleEventDccNeedFileAck(struct gg_event *e);
	void handleEventDccNeedVoiceAck(struct gg_event *e);
	void handleEventDccVoiceData(struct gg_event *e);

private slots:
	void dcc7Accepted(struct gg_dcc7 *);
	void dcc7Rejected(struct gg_dcc7 *);

protected:
	DccManager * manager() { return Manager; }

	virtual bool checkRead();
	virtual bool checkWrite();
	virtual void socketEvent();
	virtual int timeout();

public:
	DccSocketNotifiers(GaduProtocol *protocol, DccManager *manager) :
			GaduSocketNotifiers(manager), Protocol(protocol),
			Manager(manager), FileTransfer(0), DccCheckField(0) {}

	void watchFor(struct gg_dcc *socket);
	void watchFor(struct gg_dcc7 *socket);

	UinType peerUin();

	unsigned long fileSize();
	unsigned long transferredFileSize();
	QString remoteFileName();

	void setGaduFileTransfer(GaduFileTransfer *fileTransfer);
	void acceptFileTransfer();
	void rejectFileTransfer();

signals:
	void done(bool ok);

	void incomingConnection(struct gg_dcc *incomingConnection);
	void callbackReceived(DccSocketNotifiers *);

};

#ifndef _MSC_VER
#pragma GCC visibility pop
#endif

#endif // DCC_SOCKET_NOTIFIERS_H
