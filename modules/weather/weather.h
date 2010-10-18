/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef WEATHER_H
#define WEATHER_H

#include <QtCore/QObject>

#include "action.h"
#include "config_file.h"
#include "configuration_aware_object.h"
#include "userbox.h"

#include "autodownloader.h"

class QListViewItem;
class WeatherCfgUiHandler;

/**
	G��wna klasa modu�u, obs�uga menu oraz konfiguracji.
**/
class Weather : public QObject, ConfigurationAwareObject
{
	Q_OBJECT
	
	WeatherCfgUiHandler *cfgHandler_;
	AutoDownloader autoDownloader_;
	ActionDescription *actionLocalWeather_;
	ActionDescription *actionWeatherFor_;
	ActionDescription *actionContactWeather_;
	bool menuLocalWeather_;
	bool menuWeatherFor_;
	bool menuContactWeather_;

	virtual void configurationUpdated();

private slots:
	void ShowContactWeather();
	void ShowLocalWeather();
	void ShowWeatherFor();

public:
	Weather();
	~Weather();
};

#endif // WEATHER_H
