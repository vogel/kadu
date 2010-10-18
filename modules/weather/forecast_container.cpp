/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "config_file.h"
#include "debug.h"

#include "forecast_container.h"
#include "weather_global.h"

/**
	ForecastContener
**/

const Forecast * ForecastContainer::getForecast(const QString &configFile, const QString &location)
{
	kdebugf();
	
	for (Forecasts::iterator it = forecasts_.begin(); it != forecasts_.end(); it++)
	{
		if ((*it).LocationID == location && (*it).config == configFile)
		{
			if ((*it).loadTime.elapsed() > WeatherGlobal::KEEP_FORECAST * 60 * 60 * 1000)
			{
				forecasts_.erase(it);
				return 0;
			}
			else
				return &(*it);
		}
	}
	
	kdebugf2();
	return 0;
}

void ForecastContainer::deleteObsolete()
{
	kdebugf();
	
	for (Forecasts::iterator it = forecasts_.begin(); it != forecasts_.end();)
	{
		if ((*it).loadTime.elapsed() > WeatherGlobal::KEEP_FORECAST * 60 * 60 * 1000)
			it = forecasts_.erase(it);
		else
			it++;
	}
	
	kdebugf2();
}

void ForecastContainer::add(const Forecast &forecast)
{
	kdebugf();
	
	if (WeatherGlobal::KEEP_FORECAST > 0)
	{
		bool replace = false;
		for (Forecasts::iterator it = forecasts_.begin(); it != forecasts_.end(); it++)
		{
			if ((*it).LocationID == forecast.LocationID && (*it).config == forecast.config)
			{
				(*it) = forecast;
				replace = true;
				break;
			}
		}
		
		if (!replace)
			forecasts_.push_front(forecast);
	}
	
	kdebugf2();
}
