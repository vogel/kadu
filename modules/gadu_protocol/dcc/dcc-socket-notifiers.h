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

class DccSocketNotifiers : public GaduSocketNotifiers
{
	Q_OBJECT

	GaduProtocol *Protocol;
	DccManager *Manager;

	DccVersion Version;
	struct gg_dcc *Socket;
	struct gg_dcc7 *Socket7;
	int *DccCheckField;

private slots:
	void dcc7Accepted(struct gg_dcc7 *);
	void dcc7Rejected(struct gg_dcc7 *);

protected:
	DccManager * manager() { return Manager; }

	virtual bool checkRead();
	virtual bool checkWrite();
	virtual void socketEvent();
	virtual void handleEvent(struct gg_event *e);

public:
	DccSocketNotifiers(GaduProtocol *protocol, DccManager *manager, QObject *parent = 0) :
			GaduSocketNotifiers(parent), Protocol(protocol), Manager(manager), DccCheckField(0) {}

	void watchFor(struct gg_dcc *socket);
	void watchFor(struct gg_dcc7 *socket);

};

#ifndef _MSC_VER
#pragma GCC visibility pop
#endif

#endif // DCC_SOCKET_NOTIFIERS_H
