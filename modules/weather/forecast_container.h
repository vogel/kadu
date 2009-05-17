/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef FORECAST_CONTAINER_H
#define FORECAST_CONTAINER_H

#include <QtCore/QVector>

#include "forecast.h"

/**
	\class ForecastContener
	Ma za zadanie przechowywa� sciagnietej prognozy.
	Prognozy identyfikowane s� poprzez kod miejscowosci oraz
	serwer (nazwa pliku konfiguracyjnego). Oznacza to, �e w kontenerze
	nie ma dw�ch prognoz dla tej samej miejscowosci z tym samym serwerem.
	
	Funkcja deleteObsolete() usuwa z pojemnika przestarza�e prognozy
**/
class ForecastContainer
{
	typedef QVector<Forecast> Forecasts;
	Forecasts forecasts_;

public:
	const Forecast* getForecast( const QString& configFile, const QString& location );
	void deleteObsolete();
	void add( const Forecast& forecast );
};

#endif // FORECAST_CONTAINER_H
