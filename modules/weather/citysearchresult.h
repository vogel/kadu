/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CITYSEARCHRESULT_H
#define CITYSEARCHRESULT_H

#include <QtCore/QVector>

#include "userlistelement.h"

struct CitySearchResult
{
	QString cityName_;
	QString cityId_;
	QString server_;
	
	CitySearchResult() {}
	
	CitySearchResult( const QString& cityName, const QString& cityId, const QString& server )
		: cityName_(cityName), cityId_(cityId), server_(server) {}
		
	CitySearchResult( const CitySearchResult& r )
	{
		cityName_ = r.cityName_;
		cityId_ = r.cityId_;
		server_ = r.server_;
	}
	
	bool writeUserWeatherData( UserListElement user ) const;
	bool readUserWeatherData( UserListElement user );
	bool writeMyWeatherData() const;
	bool readMyWeatherData();
};

typedef QVector<CitySearchResult> CITYSEARCHRESULTS;

#endif
