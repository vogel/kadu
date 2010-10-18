/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QTextCodec>

#include "message_box.h"
#include "misc.h"

#include "get_forecast.h"
#include "weather_global.h"

/**
	GetForecast
**/
GetForecast::GetForecast()
:
	wConfig_(0)
{
	timerTimeout_ = new QTimer(this);

	connect(&httpClient_, SIGNAL(finished()), this, SLOT(downloadingFinished()));
	connect(&httpClient_, SIGNAL(error()), this, SLOT(downloadingError()));
	connect(timerTimeout_, SIGNAL(timeout()), this, SLOT(connectionTimeout()));
}

GetForecast::~GetForecast()
{
	disconnect(timerTimeout_, SIGNAL(timeout()), this, SLOT(connectionTimeout()));
	disconnect(&httpClient_, SIGNAL(finished()), this, SLOT(downloadingFinished()));
	disconnect(&httpClient_, SIGNAL(error()), this, SLOT(downloadingError()));

	if (wConfig_)
		delete wConfig_;
}

void GetForecast::downloadForecast(const QString &configFile, const QString &locID)
{
	kdebugf();

	const Forecast* savedForecast = weather_global->savedForecast_.getForecast(configFile, locID);

	if (savedForecast != 0)
	{
		forecast_ = *savedForecast;
		emit finished();
	}
	else if (!configFile.isNull())
	{
		forecast_.Days.clear();
		forecast_.LocationName = "";
		forecast_.config = configFile;
		forecast_.loadTime = QTime();
		forecast_.LocationID = locID;
	
		if (wConfig_ != 0)
			delete wConfig_;

		wConfig_ = new PlainConfigFile(WeatherGlobal::getConfigPath(forecast_.config));
		
		forecast_.serverName = wConfig_->readEntry("Header","Name");
		
		decoder_ = QTextCodec::codecForName(wConfig_->readEntry("Default","Encoding").ascii());
		
		host_ = wConfig_->readEntry("Default","Default host");
		httpClient_.setHost(host_);
		url_.sprintf(wConfig_->readEntry("Default","Default path").ascii(), locID.ascii());
		
		timerTimeout_->start(weather_global->CONNECTION_TIMEOUT, false);
		timeoutCount_ = weather_global->CONNECTION_COUNT;

		httpClient_.get(url_);
	}

	kdebugf2();
}

void GetForecast::downloadingFinished()
{
	kdebugf();
	
	timerTimeout_->stop();
	
	const QByteArray &data = httpClient_.data();
	QString page = decoder_->toUnicode(data.data(), data.count());
	
	if (!parser_.getData( page, wConfig_, forecast_))
	{
		emit error(Parser, host_ + '/' + url_);
		return;
	}
	
	forecast_.loadTime.start();
	weather_global->savedForecast_.add(forecast_);
	
	emit finished();

	kdebugf2();
}


void GetForecast::downloadingRedirected(QString link)
{
	kdebugf();
	
	splitUrl(link, host_, url_);
	httpClient_.setHost(host_);
	httpClient_.get(url_);

	kdebugf2();
}

void GetForecast::downloadingError()
{
	kdebugf();
	
	timerTimeout_->stop();
	emit error(Connection, host_ + '/' + url_);

	kdebugf2();
}

void GetForecast::connectionTimeout()
{
	kdebugf();
	
	timeoutCount_ = timeoutCount_ - 1;
	if (timeoutCount_ <= 0)
	{
		httpClient_.setHost("");
		downloadingError();
	}
	else
	{
		httpClient_.setHost("");
		httpClient_.setHost(host_);
		httpClient_.get(url_);
	}
	
	//MessageBox::msg("connectionTimeout()");
	kdebugf2();
}

void GetForecast::splitUrl(const QString &url_, QString &host_, QString &path) const
{
	int endhost = url_.find('/');
	host_ = url_.left(endhost);
	path = url_.right(url_.length()-endhost);
}
