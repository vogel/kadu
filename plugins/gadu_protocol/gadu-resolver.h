/*
 * %kadu copyright begin%
 * Copyright 2008, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#ifndef GADU_RESOLVER_H
#define GADU_RESOLVER_H

#include <QtCore/QObject>

struct gadu_resolver_data
{
	int rfd; /*< Read descriptor */
	int wfd; /*< Write descriptor */
};

class QHostAddress;
class QHostInfo;
class QTimer;

class GaduResolver : public QObject
{
	Q_OBJECT

	QTimer *Timer;
	int LookupId;
	gadu_resolver_data *Data;

	void reset();

private slots:
	void abort();
	void resolved(const QHostInfo &host);

public:
	explicit GaduResolver(gadu_resolver_data *data, QObject *parent = 0);
	virtual ~GaduResolver();

	void resolve(const QString &hostname);

};

int gadu_resolver_start(int *fd, void **priv_data, const char *hostname);
void gadu_resolver_cleanup(void **priv_data, int force);

#endif // GADU_RESOLVER_H
