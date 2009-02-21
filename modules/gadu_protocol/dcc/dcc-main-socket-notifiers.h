/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef DCC_MAIN_SOCKET_NOTIFIERS_H
#define DCC_MAIN_SOCKET_NOTIFIERS_H

#include <libgadu.h>

#include "dcc/dcc-socket-notifiers.h"

#ifndef _MSC_VER
#pragma GCC visibility push(hidden)
#endif

class DccMainSocketNotifiers : public DccSocketNotifiers
{
	Q_OBJECT

protected:
	virtual void handleEvent(struct gg_event *e);

public:
	DccMainSocketNotifiers(GaduProtocol *protocol, DccManager *manager, QObject *parent = 0) :
			DccSocketNotifiers(protocol, manager, parent) {}

};

#ifndef _MSC_VER
#pragma GCC visibility pop
#endif

#endif // DCC_MAIN_SOCKET_NOTIFIERS_H
