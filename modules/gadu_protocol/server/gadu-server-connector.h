/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef GADU_SERVER_CONNECTOR_H
#define GADU_SERVER_CONNECTOR_H

#include <QtCore/QObject>
#include <QtGui/QPixmap>

#include "misc/token-reader.h"

class GaduServerConnector : public QObject
{
	Q_OBJECT

	TokenReader *Reader;
	bool Result;

private slots:
	void tokenFetched(const QString &, const QPixmap &);

protected:
	virtual void performAction(const QString &tokenId, const QString &tokenValue) = 0;

	void finished(bool result);

public:
	GaduServerConnector(TokenReader *reader)
			: Reader(reader) {}

	void perform();
	bool result() { return Result; }

signals:
	void finished(GaduServerConnector *);

};

#endif // GADU_SERVER_CONNECTOR_H
