/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include <QtGui/QListWidget>
#include <QtGui/QSpinBox>
#include <QtGui/QComboBox>
#include <QtGui/QCheckBox>

#include "kadu.h"
#include "exports.h"
#include "userbox.h"
#include "userlist.h"
#include "main_configuration_window.h"
#include "configuration_window_widgets.h"
#include "message_box.h"
#include "icons_manager.h"
#include "modules/notify/notify.h"

#include "weather.h"
#include "show_forecast_dialog.h"
#include "getcitydialog.h"
#include "weather_global.h"
#include "citysearchresult.h"
#include "weather_cfg_ui_handler.h"

WeatherGlobal* weather_global;
Weather* weather_session;

extern "C" KADU_EXPORT int weather_init(bool firstLoad)
{
	weather_global = new WeatherGlobal;
    weather_session = new Weather;

    return 0;
}

extern "C" KADU_EXPORT void weather_close()
{
	delete weather_session;
	delete weather_global;
}

/**
	Weather
**/
Weather::Weather()
{
	// Contacts parameters
	userlist->addPerContactNonProtocolConfigEntry("city", "City");
	userlist->addPerContactNonProtocolConfigEntry("weather", "WeatherData");

	// Config parameters
	//
	config_file.addVariable("Weather", "HintFont", QFont("Helvetica", 11));
	config_file.addVariable("Weather", "HintDay", 0);
	config_file.addVariable("Weather", "DescriptionDay", 0);
	config_file.addVariable("Weather", "HintText", tr("<u>%l</u> - <b>%d:</b><br>Temperature: %t<br>Pressure: %p"));
	config_file.addVariable("Weather", "DescriptionText", tr("Temperature in %l: %t"));

	cfgHandler_ = new WeatherCfgUiHandler;

	notification_manager->registerEvent("NewForecast",  QT_TRANSLATE_NOOP("@default", "New forecast has been fetched"), CallbackNotRequired);

	actionLocalWeather_ = new ActionDescription(ActionDescription::TypeGlobal, "LocalWeather", this, SLOT(ShowLocalWeather()),
		"ShowWeather", tr("Local forecast"));
	actionWeatherFor_ = new ActionDescription(ActionDescription::TypeGlobal, "WeatherFor", this, SLOT(ShowWeatherFor()),
		"ShowWeather", tr("Forecast for..."));
	actionContactWeather_ = new ActionDescription(ActionDescription::TypeGlobal, "ContactWeather", this, SLOT(ShowContactWeather()),
		"ShowWeather", tr("Show contact weather"));

	menuLocalWeather_ = config_file.readBoolEntry("Weather", "ShowLocalForecast", true);
	menuWeatherFor_ = config_file.readBoolEntry("Weather", "ForecastFor", true);
	menuContactWeather_ = config_file.readBoolEntry("Weather", "ShowContactWeather", true);

	if (menuLocalWeather_)
		kadu->insertMenuActionDescription(0, actionLocalWeather_);
	if (menuWeatherFor_ )
		kadu->insertMenuActionDescription(0, actionWeatherFor_);
	if (menuContactWeather_ )
		UserBox::insertActionDescription(0, actionContactWeather_);
}

Weather::~Weather()
{
	delete cfgHandler_;
 	notification_manager->unregisterEvent("NewForecast");

	if (menuLocalWeather_)
		kadu->removeMenuActionDescription(actionLocalWeather_);
	if (menuWeatherFor_)
		kadu->removeMenuActionDescription(actionWeatherFor_);
	if (menuContactWeather_)
		UserBox::removeActionDescription(actionContactWeather_);
}

void Weather::configurationUpdated()
{
	if (config_file.readBoolEntry("Weather", "ShowLocalForecast") && !menuLocalWeather_)
	{
		kadu->addMenuActionDescription(actionLocalWeather_);
		menuLocalWeather_ = true;
	}
	else if (!config_file.readBoolEntry("Weather", "ShowLocalForecast") && menuLocalWeather_)
	{
		kadu->removeMenuActionDescription(actionLocalWeather_);
		menuLocalWeather_ = false;
	}

	if (config_file.readBoolEntry("Weather", "ForecastFor") && !menuWeatherFor_)
	{
		kadu->addMenuActionDescription(actionWeatherFor_);
		menuWeatherFor_ = true;
	}
	else if (!config_file.readBoolEntry("Weather", "ForecastFor") && menuWeatherFor_)
	{
		kadu->removeMenuActionDescription(actionWeatherFor_);
		menuWeatherFor_ = false;
	}

	if (config_file.readBoolEntry("Weather", "ShowContactWeather") && !menuContactWeather_)
	{
		UserBox::addActionDescription(actionContactWeather_);
		menuContactWeather_ = true;
	}
	else if (!config_file.readBoolEntry("Weather", "ShowContactWeather") && menuContactWeather_)
	{
		UserBox::removeActionDescription(actionContactWeather_);
		menuContactWeather_ = false;
	}
}

void Weather::ShowContactWeather()
{
	UserBox *userBox = kadu->userbox();
	if (userBox != 0)
	{
		UserListElement user = userBox->selectedUsers().at(0);

		CitySearchResult result;
		if (result.readUserWeatherData(user))
		{
			ShowForecastDialog *sfd = new ShowForecastDialog(result, user);
			sfd->show();
		}
		else
		{
			SearchingCityDialog *scd = new SearchingCityDialog( user );
			scd->show();
		}
	}
}

void Weather::ShowLocalWeather()
{
	CitySearchResult result;
	if (result.readMyWeatherData())
	{
		ShowForecastDialog *sfd = new ShowForecastDialog(result, kadu->myself());
		sfd->show();
	}
	else
	{
		SearchingCityDialog *scd = new SearchingCityDialog(kadu->myself());
		scd->show();
	}

}

void Weather::ShowWeatherFor()
{
	EnterCityDialog *window = new EnterCityDialog;
	window->show();
}
