/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef DCC_SOCKET_H
#define DCC_SOCKET_H

#include <qobject.h>

#include "dcc.h"

class QSocketNotifier;

class DccHandler;

class DccSocket : public QObject
{
	Q_OBJECT

	DccVersion Version;

	struct gg_dcc *Dcc6Struct;
	struct gg_dcc7 *Dcc7Struct;
	int &DccCheckField;

	struct gg_event *DccEvent;

	QSocketNotifier* ReadSocketNotifier;
	QSocketNotifier* WriteSocketNotifier;

	bool ConnectionClosed;

	void initializeNotifiers();
	void finalizeNotifiers();

	bool dccClientAccept();

private slots:
	void socketDataEvent();

	void dcc7Accepted(struct gg_dcc7 *);
	void dcc7Rejected(struct gg_dcc7 *);

protected:
	DccHandler *Handler;

	void enableNotifiers();
	void disableNotifiers();

	void connectionError();
	void closeSocket(bool error);

	void watchDcc();
	bool checkRead();
	bool checkWrite();

public:
	DccSocket(struct gg_dcc *dccStruct);
	DccSocket(struct gg_dcc7 *dccStruct);
	~DccSocket();

	struct gg_event * ggDccEvent() const;
	UinType uin();
	UinType peerUin();
	void setType(int type);

	unsigned char * fileName();
	int fileSize();
	int fileOffset();
	void fillFileInfo(const QString &fileName);

	bool setFile(int fd);
	void setOffset(long offset);

	void setHandler(DccHandler *handler);

	void discard();

	bool connectionClosed() { return ConnectionClosed; }

};

#endif // DCC_SOCKET_H
