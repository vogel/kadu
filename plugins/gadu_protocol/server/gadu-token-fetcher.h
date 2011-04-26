/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Tomasz Rostanski (rozteck@interia.pl)
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

#ifndef GADU_TOKEN_FETCHER
#define GADU_TOKEN_FETCHER

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtGui/QPixmap>
#include <QtNetwork/QHttp>

class GaduTokenFetcher : public QObject
{
	Q_OBJECT

	QHttp Http;
	QString TokenId;

private slots:
	void tokenReceivedSlot(int id, bool error);

public:
	explicit GaduTokenFetcher(QObject *parent = 0);
	virtual ~GaduTokenFetcher();

	void fetchToken();

signals:
	void tokenFetched(const QString &tokenId, const QPixmap &tokenPixmap);

};

#endif // GADU_TOKEN_FETCHER
