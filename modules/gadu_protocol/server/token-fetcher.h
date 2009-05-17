/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TOKEN_FETCHER
#define TOKEN_FETCHER

#include <QtCore/QObject>
#include <QtGui/QPixmap>

struct gg_http;

class TokenFetcher : public QObject
{
	Q_OBJECT

	gg_http *H;
	bool AutoDelete;

private slots:
	void tokenFetchedSlot(const QString &tokenId, QPixmap tokenPixmap);

public:
	explicit TokenFetcher(bool autoDelete, QObject *parent = 0);
	virtual ~TokenFetcher();

	void fetchToken();

signals:
	void tokenFetched(const QString &tokenId, QPixmap tokenPixmap);

};

#endif // TOKEN_FETCHER
