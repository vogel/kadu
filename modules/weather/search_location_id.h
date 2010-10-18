/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SEARCH_LOCATION_ID_H
#define SEARCH_LOCATION_ID_H

#include <QtCore/QMap>

#include "http_client.h"
#include "misc.h"
#include "config_file.h"

#include "weather_parser.h"
#include "weather_global.h"
#include "citysearchresult.h"

/**
	\class SearchLocationID
	Klasa zajmuj�ca si� wyszukiwaniem kodu miejscowo�ci.
	Wyszukiwanie inicujuj si� przez wywo�anie findID(), a obs�ug� realizuj� gniazda
	finished(), redirected() i error()
**/
class SearchLocationID : public QObject
{
	Q_OBJECT

	HttpClient httpClient_;
	QTextCodec *decoder_;
	QString host_;
	QString url_;
	WeatherParser parser_;
	PlainConfigFile *weatherConfig_;
	QString serverConfigFile_;

	WeatherGlobal::SERVERITERATOR currentServer_;
	bool searchAllServers_;
	bool redirected_;

	QTimer *timerTimeout_;
	int timeoutCount_;

	QString city_;

	CITYSEARCHRESULTS results_;

	void encodeUrl(QString *str, const QString &enc) const;
	void splitUrl(const QString &url, QString &host, QString &path) const;
	void findNext(const QString &serverConfigFile);
	void findNext();

private slots:
	void downloadingFinished();
	void downloadingRedirected(QString link);
	void downloadingError();
	void connectionTimeout();

public:
	SearchLocationID();
	~SearchLocationID();

	bool findID(const QString &city, const QString &serverConfigFile);
	bool findID(const QString &city);

	void cancel();
	const CITYSEARCHRESULTS & getResult() const { return results_;}

signals:
	void finished();
	void error(QString url);
	void nextServerSearch(const QString &city, const QString &serverName_);

};

#endif // SEARCH_LOCATION_ID_H
