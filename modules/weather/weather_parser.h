/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef WEATHER_PARSER_H
#define WEATHER_PARSER_H

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QDateTime>
#include <QtCore/QTimer>

#include "config_file.h"
#include "debug.h"

#include "forecast.h"
#include "citysearchresult.h"

/**
	\class WeatherParser
	Zajmuje sie wylowieniem informacji pogodowych ze strony WWW.
**/
class WeatherParser
{
	struct WDataValue
	{
		QString Name;
		QString Start;
		QString End;
		QString Content;
	};

	bool getDataValue(const QString &page, WDataValue &wdata, int &cursor, const PlainConfigFile *wConfig, bool CaseSensitive) const;
	QString tagClean(QString str) const;

public:
	WeatherParser();

	bool getData(const QString &page, const PlainConfigFile *wConfig, Forecast &forecast) const;

	/**
			Parsuje wyniki wyszukiwania
			\param page pobrana witryna
			\param wConfig konfiguracja serwisu pogodowego
			\param result wyniki parsowania: klucz - nazwa, dana - kod miejscowo�ci
		**/
	void getSearch(const QString &page, const PlainConfigFile *wConfig, const QString &serverConfigFile , CITYSEARCHRESULTS *results ) const;
	QString getFastSearch(const QString &link, const PlainConfigFile *wConfig) const;
};

#endif // WEATHER_PARSER_H
