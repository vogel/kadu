/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QTimer>

#include "config_file.h"
#include "modules/notify/notify.h"
#include "debug.h"

#include "autodownloader.h"
#include "weather_global.h"

WeatherStatusChanger::WeatherStatusChanger() :
	StatusChanger(900), enabled_(false)
{
}

void WeatherStatusChanger::setDescription(const QString &description)
{
	if (description_ != description)
	{
		description_ = description;
		if (enabled_)
			emit statusChanged();
	}
}

void WeatherStatusChanger::changeStatus(UserStatus &status)
{
	if (enabled_)
	{
		switch (config_file.readNumEntry("Weather", "DescriptionPos"))
		{
			case 0:
				status.setDescription(description_);
				break;
			case 1:
				status.setDescription(description_ + ' ' + status.description());
				break;
			case 2:
				status.setDescription(status.description() + ' ' + description_);
				break;
			case 3:
			default:
				status.setDescription(status.description().replace("%weather%", description_));
				break;
		}
	}
}

void WeatherStatusChanger::setEnabled(bool enabled)
{
	if (enabled_ != enabled)
	{
		enabled_ = enabled;
		emit statusChanged();
	}
}

AutoDownloader::AutoDownloader() :
	autoDownloadTimer_(new QTimer(this)),
	downloader_(new GetForecast()),
	statusChanger_(new WeatherStatusChanger()),
	fetchingEnabled_(config_file.readBoolEntry("Weather", "bAuto")),
	hintsEnabled_(config_file.readBoolEntry("Weather", "bHint")),
	descEnabled_(config_file.readBoolEntry("Weather", "bDescription"))
{
	status_changer_manager->registerStatusChanger(statusChanger_.get());

	connect(autoDownloadTimer_, SIGNAL(timeout()), this, SLOT(autoDownload()));
	connect(downloader_.get(), SIGNAL(finished()), this, SLOT(autoDownloadingFinished()));

	if (WeatherGlobal::KEEP_FORECAST > 0 && fetchingEnabled_)
	{
		autoDownloadTimer_->start(WeatherGlobal::KEEP_FORECAST * 60 * 60 * 1000, false);
		autoDownload();
	}
}

AutoDownloader::~AutoDownloader()
{
	status_changer_manager->unregisterStatusChanger(statusChanger_.get());
}

void AutoDownloader::configurationUpdated()
{
	kdebugf();

	bool newFetchingEnabled = config_file.readBoolEntry("Weather", "bAuto");
	bool newHintsEnabled = config_file.readBoolEntry("Weather", "bHint");
	bool newDescEnabled = config_file.readBoolEntry("Weather", "bDescription");

	if (!newFetchingEnabled || !newDescEnabled)
		statusChanger_->setEnabled(false);

	if (!newFetchingEnabled && autoDownloadTimer_->isActive())
		autoDownloadTimer_->stop();
	else if (newFetchingEnabled &&
		((newFetchingEnabled  && !fetchingEnabled_) ||
		(newHintsEnabled &&  !hintsEnabled_) ||
		(newDescEnabled && !descEnabled_)))
	{
		autoDownloadTimer_->start(WeatherGlobal::KEEP_FORECAST * 60 * 60 * 1000, false);
		autoDownload();
	}

	fetchingEnabled_ = newFetchingEnabled;
	hintsEnabled_ = newHintsEnabled;
	descEnabled_ = newDescEnabled;

	kdebugf2();
}

QString AutoDownloader::parse(const ForecastDay& day, QString str)
{
	str.replace("%l", downloader_->getForecast().LocationName);
	str.replace("%T", downloader_->getForecast().loadTime.toString( "h:mm" ));
	str.replace("%d", day["Name"]);
	str.replace("%t", day["Temperature"]);
	str.replace("%o", day["Description"]);
	str.replace("%w", day["Wind speed"]);
	str.replace("%p", day["Pressure"]);
	str.replace("&deg;","�");
	str.replace("&nbsp;"," ");
	return str;
}

void AutoDownloader::autoDownload()
{
	kdebugf();

	QString server = config_file.readEntry("Weather", "MyServer");
	QString cityId = config_file.readEntry("Weather", "MyCityId");

	if (server.isEmpty() || cityId.isEmpty())
		return;

	downloader_->downloadForecast(server, cityId);

	kdebugf2();
}

void AutoDownloader::autoDownloadingFinished()
{
	kdebugf();

	if (WeatherGlobal::KEEP_FORECAST > 0 && config_file.readBoolEntry("Weather", "bAuto"))
	{
		if (config_file.readBoolEntry("Weather", "bHint"))
		{
			const ForecastDay& day = downloader_->getForecast().Days[config_file.readNumEntry("Weather", "HintDay")];

			Notification* notification = new Notification( "NewForecast", day["Icon"], UserListElements());
			notification->setTitle(tr("New forecast has been fetched") );
			notification->setText(parse(day, config_file.readEntry("Weather", "HintText")));
			notification_manager->notify( notification );
		}

		if (config_file.readBoolEntry("Weather", "bDescription"))
		{
			const ForecastDay& day = downloader_->getForecast().Days[config_file.readNumEntry("Weather", "DescriptionDay")];
			QString description = parse(day, config_file.readEntry("Weather", "DescriptionText"));
			statusChanger_->setDescription(description);
			statusChanger_->setEnabled(true);
		}
	}

	kdebugf2();
}
