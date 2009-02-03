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

#include "gadu-socket-notifiers.h"

#ifndef _MSC_VER
#pragma GCC visibility push(hidden)
#endif

class GaduPubdirSocketNotifiers : public GaduSocketNotifiers
{
	Q_OBJECT

	struct gg_http *H;

protected:
	virtual void socketEvent();

protected slots:
	virtual void dataReceived();
	virtual void dataSent();

public:
	GaduPubdirSocketNotifiers(struct gg_http *, QObject *parent = 0);
	virtual ~GaduPubdirSocketNotifiers();

signals:
	void done(bool ok, struct gg_http *);

};

#ifndef _MSC_VER
#pragma GCC visibility pop
#endif

#endif // GADU_PUBDIR_SOCKET_NOTIFIERS_H
