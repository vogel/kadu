/*
 * %kadu copyright begin%
 * Copyright 2016 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "paste-acceptor.h"

#include <QtCore/QMimeData>
#include <QtCore/QUrl>

void acceptPasteData(const QMimeData *source, PasteAcceptor *acceptor)
{
	if (source->hasFormat(QStringLiteral("text/plain")))
	{
		acceptor->acceptPlainText(source->text());
		return;
	}

	if (!source->urls().isEmpty())
	{
		auto url = source->urls().first();
		if (url.isEmpty() || url.scheme() != "file")
			return;
		acceptor->acceptFileUrl(url);
		return;
	}

	if (source->hasFormat(QStringLiteral("application/x-qt-image")))
		acceptor->acceptImageData(source->data(QStringLiteral("application/x-qt-image")));
}
