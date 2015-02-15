/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef GADU_PUBDIR_SOCKET_NOTIFIERS_H
#define GADU_PUBDIR_SOCKET_NOTIFIERS_H

#include <libgadu.h>

#include "gadu-socket-notifiers.h"

class GaduPubdirSocketNotifiers : public GaduSocketNotifiers
{
	Q_OBJECT

	struct gg_http *H;

	void finished(bool ok);

protected:
	virtual bool checkRead();
	virtual bool checkWrite();
	virtual void socketEvent();
	virtual int timeout();
	virtual bool handleSoftTimeout();
	virtual void connectionTimeout();

public:
	GaduPubdirSocketNotifiers(QObject *parent = 0) :
			GaduSocketNotifiers(parent), H(0) {}

	void watchFor(struct gg_http *h);

signals:
	void done(bool ok, struct gg_http *h);

};

#endif // GADU_PUBDIR_SOCKET_NOTIFIERS_H
