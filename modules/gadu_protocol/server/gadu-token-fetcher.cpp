/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QUrl>

#include <libgadu.h>

#include "debug.h"

#include "gadu-token-fetcher.h"

GaduTokenFetcher::GaduTokenFetcher(QObject *parent) :
		QObject(parent)
{
}

GaduTokenFetcher::~GaduTokenFetcher()
{
}

void GaduTokenFetcher::fetchToken()
{
	TokenId.clear();

	QHttpRequestHeader header("POST", "/appsvc/regtoken.asp");
	header.setValue("Host", GG_REGISTER_HOST);
	header.setValue("User-Agent", GG_HTTP_USERAGENT);
	header.setValue("Content-Type", "application/x-www-form-urlencoded");
	header.setValue("Content-Length", "0");
	header.setValue("Pragma", "no-cache");
	Http.setHost(GG_REGISTER_HOST, GG_REGISTER_PORT);
	Http.request(header);
	connect(&Http, SIGNAL(requestFinished(int, bool)), this, SLOT(tokenReceivedSlot(int, bool)));
}

void GaduTokenFetcher::tokenReceivedSlot(int id, bool error)
{
	Q_UNUSED(id)
	Q_UNUSED(error)

	QByteArray data = Http.readAll();
	if (data.size() == 0)
		return;

	if (TokenId.isEmpty())
	{
		QStringList list = QString(data).split(QRegExp("[\r\n ]"), QString::SkipEmptyParts);
		if (list.size() != 5)
		{
			kdebugm(KDEBUG_INFO, "Empty response. Retrying\n");
			fetchToken();
			return;
		}
		TokenId = list.at(3);
		QString url = list.at(4);

		Http.get(url + "?tokenid=" + TokenId);
	}
	else
	{
		QPixmap tokenImage;
		tokenImage.loadFromData(data);
		emit tokenFetched(TokenId, tokenImage);
		TokenId.clear();
	}
}
