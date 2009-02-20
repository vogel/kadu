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

#include "gadu-socket-notifiers.h"

#ifndef _MSC_VER
#pragma GCC visibility push(hidden)
#endif

class GaduTokenSocketNotifiers : public GaduSocketNotifiers
{
	Q_OBJECT

	struct gg_http *H;

protected:
	virtual bool checkRead();
	virtual bool checkWrite();
	virtual void socketEvent();

public:
	GaduTokenSocketNotifiers(struct gg_http *h, QObject *parent = 0);
	virtual ~GaduTokenSocketNotifiers();

signals:
	void gotToken(const QString &, const QPixmap &);
	void tokenError();

};

#ifndef _MSC_VER
#pragma GCC visibility pop
#endif

#endif // GADU_TOKEN_SOCKET_NOTIFIERS_H
