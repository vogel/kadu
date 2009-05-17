/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "config_file.h"
#include "kadu.h"

#include "citysearchresult.h"
#include "weather_global.h"

bool CitySearchResult::writeUserWeatherData( UserListElement user ) const
{
	if( user == kadu->myself() )
	{
		return writeMyWeatherData();
	}
	else
	{
		if( cityName_.isEmpty() || server_.isEmpty() || cityId_.isEmpty() )
			return false;
		
		user.setData( "City", cityName_ );
		user.setData( "WeatherData", server_ + ';' + cityId_ );
		return true;
	}
}

bool CitySearchResult::readUserWeatherData( UserListElement user )
{
	if( user == kadu->myself() )
	{
		return readMyWeatherData();
	}
	else
	{
		cityName_ = user.data( "City" ).toString();
		if( cityName_.isEmpty() )
			return false;
		
		QString weatherData = user.data( "WeatherData" ).toString();
		if( weatherData.isEmpty() || !weatherData.contains(';') )
			return false;
		
		int endServer = weatherData.find(';');
		server_ = weatherData.left( endServer );
		cityId_ = weatherData.right( weatherData.length() - endServer - 1 );
		
		return( !cityId_.isEmpty() && weather_global->configFileExists( server_ ) );
	}
}

bool CitySearchResult::writeMyWeatherData() const
{
	if( cityName_.isEmpty() || server_.isEmpty() || cityId_.isEmpty() )
		return false;
	
	config_file.writeEntry( "Weather", "MyCity", cityName_ );
	config_file.writeEntry( "Weather", "MyServer", server_ );
	config_file.writeEntry( "Weather", "MyCityId", cityId_ );
	return true;
}

bool CitySearchResult::readMyWeatherData()
{
	cityName_ = config_file.readEntry( "Weather", "MyCity" );
	server_ = config_file.readEntry( "Weather", "MyServer" );
	cityId_ = config_file.readEntry( "Weather", "MyCityId" );
	return( !cityName_.isEmpty() && !cityId_.isEmpty() && weather_global->configFileExists( server_ ) );
}
