/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GADU_TOKEN_SOCKET_NOTIFIERS_H
#define GADU_TOKEN_SOCKET_NOTIFIERS_H

#include <QtGui/QPixmap>

#include <libgadu.h>

#include "gadu-socket-notifiers.h"

#ifndef _MSC_VER
#pragma GCC visibility push(hidden)
#endif

class GaduTokenSocketNotifiers : public GaduSocketNotifiers
{
	Q_OBJECT

	struct gg_http *H;

	void finished(const QString &tokenId, const QPixmap &tokenPixmap);

protected:
	virtual bool checkRead();
	virtual bool checkWrite();
	virtual void socketEvent();
	virtual int timeout();

public:
	GaduTokenSocketNotifiers(QObject *parent = 0)
		: GaduSocketNotifiers(parent), H(0) {}

	void watchFor(struct gg_http *h);

signals:
	void done(const QString &, const QPixmap &);

};

#ifndef _MSC_VER
#pragma GCC visibility pop
#endif

#endif // GADU_TOKEN_SOCKET_NOTIFIERS_H
