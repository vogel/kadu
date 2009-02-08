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

#include "token-reader.h"

class GaduServerConnector : public QObject
{
	Q_OBJECT

	TokenReader *Reader;

private slots:
	void tokenFetched(const QString &, const QPixmap &);

protected:
	virtual void performAction(const QString &tokenId, const QString &tokenValue) = 0;

public:
	GaduServerConnector(TokenReader *reader)
		: Reader(reader) {}

	void perform();

signals:
	void result(bool);

};

#endif // GADU_SERVER_CONNECTOR_H
