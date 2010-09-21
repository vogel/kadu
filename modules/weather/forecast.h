/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef FORECAST_H
#define FORECAST_H

#include <QtCore/QMap>
#include <QtCore/QDateTime>
#include <QtCore/QVector>

/**
	Zawiera liste wartosci opisujacych pogode. Dozwolone wartosci:
	"Name", "Icon", "Temperature", "Pressure", "Rain", "Snow", "Wind Speed", "Description"
	"Humidity", "Dew point", "Visibility"
**/
typedef QMap<QString, QString> ForecastDay;

/**
	\struct Forecast
	Zawiera informacjê o jednym dniu z prognozy pogody.
	LocationName - nazwa miejscowosci
	LocationID - kod miejscowosci
	config - nazwa pliku konfiguracyjnego
	loadTime - data pobrania prognozy
	Days - poszczególne dni prognozy
**/
struct Forecast
{
	QString LocationName;
	QString LocationID;
	QVector<ForecastDay> Days;
	QString config;
	QString serverName;
	QTime loadTime;
};

#endif // FORECAST_H
