/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GADU_SERVER_CONNECTOR_H
#define GADU_SERVER_CONNECTOR_H

#include <QtCore/QObject>
#include <QtGui/QPixmap>

#include <libgadu.h>

#include "token-reader.h"

class GaduServerConnector : public QObject
{
	Q_OBJECT

	gg_http *H;

	TokenReader *Reader;
	bool Result;

private slots:
	void tokenFetched(const QString &, const QPixmap &);
	void tokenFetchFailed();

protected:
	virtual void performAction(const QString &tokenId, const QString &tokenValue) = 0;

	void finished(bool result);

public:
	GaduServerConnector(TokenReader *reader)
		: Reader(reader), H(0) {}

	void perform();
	bool result() { return Result; }

signals:
	void finished(GaduServerConnector *);

};

#endif // GADU_SERVER_CONNECTOR_H
