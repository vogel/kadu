/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GADU_PUBDIR_SOCKET_NOTIFIERS_H
#define GADU_PUBDIR_SOCKET_NOTIFIERS_H

#include <libgadu.h>

#include "gadu-socket-notifiers.h"

#ifndef _MSC_VER
#pragma GCC visibility push(hidden)
#endif

class GaduPubdirSocketNotifiers : public GaduSocketNotifiers
{
	Q_OBJECT

	struct gg_http *H;

	void finished(bool ok);

protected:
	virtual bool checkRead();
	virtual bool checkWrite();
	virtual void socketEvent();

public:
	GaduPubdirSocketNotifiers(QObject *parent = 0) :
			GaduSocketNotifiers(parent), H(0) {}

	void watchFor(struct gg_http *h);

signals:
	void done(bool ok, struct gg_http *h);

};

#ifndef _MSC_VER
#pragma GCC visibility pop
#endif

#endif // GADU_PUBDIR_SOCKET_NOTIFIERS_H
