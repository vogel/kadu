/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

	QTimer *timer;
	int id;
	gadu_resolver_data *data;

private slots:
	void abort();
	void resolved(const QHostInfo &host);

public:
	explicit GaduResolver(QObject *parent = 0);
	virtual ~GaduResolver();

	void resolve(const QString &hostname);
	void setData(gadu_resolver_data *);
	gadu_resolver_data * getData();

};

extern GaduResolver *resolver;

int gadu_resolver_start(int *fd, void **priv_data, const char *hostname);
void gadu_resolver_cleanup(void **priv_data, int force);

#endif // GADU_RESOLVER_H
