/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QLayout>
#include <QtGui/QTabWidget>

#include "show_forecast_dialog.h"
#include "citysearchresult.h"
#include "getcitydialog.h"
#include "showforecastframe.h"
#include "textprogress.h"
#include "weather_global.h"

ShowForecastDialog::ShowForecastDialog( const CitySearchResult& city, UserListElement user )
	: user_( user )
{
	setAttribute(Qt::WA_DeleteOnClose);
	resize(400, 300);
	
	ShowForecastFrameBase* page = new ShowForecastFrame( this, city );
	connect( page, SIGNAL(changeCity()), this, SLOT(changeCity()) );
	
	QTabWidget* tabs = new QTabWidget( this );
	tabs->addTab( page, weather_global->getServerName( city.server_ ) );
	connect( tabs, SIGNAL(currentChanged( QWidget* )), this, SLOT(tabChanged( QWidget* )) );
	
	QVBoxLayout* layout = new QVBoxLayout( this );
	layout->addWidget( tabs );
	
	WeatherGlobal::SERVERITERATOR serverIt = weather_global->beginServer();
	while( serverIt != weather_global->endServer() )
	{
		if( (*serverIt).configFile_ != city.server_ )
		{
			ShowForecastFrameBase* newpage = new SearchAndShowForecastFrame(this, city.cityName_, (*serverIt).configFile_);
			tabs->addTab( newpage, (*serverIt).name_ );
			connect( newpage, SIGNAL(changeCity()), this, SLOT(changeCity()) );
		}
		serverIt = weather_global->nextServer( serverIt );
	}
	
	setCaption(  tr("%1 - Forecast").arg( city.cityName_)  );
	page->start();
}

void ShowForecastDialog::tabChanged( QWidget* page )
{
	ShowForecastFrameBase* frame = reinterpret_cast<ShowForecastFrameBase*>( page );
	frame->start();
}

void ShowForecastDialog::changeCity()
{
	close();
	SearchingCityDialog* scd = new SearchingCityDialog( user_ );
	scd->show();
}
