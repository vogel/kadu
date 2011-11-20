/*
 * %kadu copyright begin%
 * Copyright 2008, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007, 2008, 2009 Dawid Stawiarski (neeo@kadu.net)
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

#ifndef DCC_SOCKET_NOTIFIERS_H
#define DCC_SOCKET_NOTIFIERS_H

#include <libgadu.h>

#include "buddies/buddy.h"

#include "socket-notifiers/gadu-socket-notifiers.h"
#include "gadu-protocol.h"

class QFile;

class GaduFileTransferHandler;

class DccSocketNotifiers : public GaduSocketNotifiers
{
	Q_OBJECT

	GaduFileTransferHandler *FileTransferHandler;

	struct gg_dcc7 *Socket7;

	void watchFor();

	void accepted();
	void rejected();
	void finished(bool ok);

	void handleEventDcc7Connected(struct gg_event *e);
	void handleEventDcc7Error(struct gg_event *e);
	void handleEventDcc7Done(struct gg_event *e);

protected:
	virtual bool checkRead();
	virtual bool checkWrite();
	virtual void socketEvent();
	virtual int timeout();
	virtual bool handleSoftTimeout();
	virtual void connectionTimeout();

public:
	explicit DccSocketNotifiers(struct gg_dcc7 *socket, QObject *parent = 0);
	virtual ~DccSocketNotifiers();

	void start();

	void handleEventDcc7Accept(struct gg_event *e);
	void handleEventDcc7Reject(struct gg_event *e);
	void handleEventDcc7Pending(struct gg_event *e);

	UinType peerUin();

	unsigned long fileSize();
	unsigned long transferredFileSize();
	QString remoteFileName();

	void setGaduFileTransferHandler(GaduFileTransferHandler *fileTransferHandler);
	bool acceptFileTransfer(const QString &fileName, bool resumeTransfer);
	void rejectFileTransfer();

signals:
	void done(bool ok);

};

#endif // DCC_SOCKET_NOTIFIERS_H
