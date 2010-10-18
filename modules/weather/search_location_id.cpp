/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QTextCodec>
#include <QtCore/QUrl>

#include "search_location_id.h"
#include "weather_global.h"

/**
	SearchLocationID
**/

SearchLocationID::SearchLocationID()
:
	weatherConfig_(0),
	searchAllServers_(false),
	redirected_(false)
{
	timerTimeout_ = new QTimer(this);

	connect(timerTimeout_, SIGNAL(timeout()), this, SLOT(connectionTimeout()));
}

SearchLocationID::~SearchLocationID()
{
	disconnect(timerTimeout_, SIGNAL(timeout()), this, SLOT(connectionTimeout()));

	if (weatherConfig_)
		delete weatherConfig_;
}

bool SearchLocationID::findID(const QString &city, const QString &serverConfigFile)
{
	kdebugf();
	
	if (city.isEmpty())
		return false;
	else
		city_ = city;
	
	searchAllServers_ = false;
	redirected_ = false;
	
	results_.clear();
	findNext(serverConfigFile);
	
	kdebugf2();
	return true;
}

bool SearchLocationID::findID(const QString &city)
{
	kdebugf();
	
	if (city.isEmpty())
		return false;
	else
		city_ = city;
	
	searchAllServers_ = true;
	redirected_ = false;
	
	currentServer_ = weather_global->beginServer();
	
	if (currentServer_ == weather_global->endServer())
		return false;
		
	emit nextServerSearch(city_, (*currentServer_).name_);
	
	results_.clear();
	findNext((*currentServer_).configFile_);
	
	kdebugf2();
	return true;
}

void SearchLocationID::findNext( const QString& serverConfigFile )
{
	kdebugf();
	
	connect(&httpClient_, SIGNAL(finished()), this, SLOT(downloadingFinished()));
	connect(&httpClient_, SIGNAL(error()), this, SLOT(downloadingError()));
	connect(&httpClient_, SIGNAL(redirected(QString)), this, SLOT(downloadingRedirected(QString)));
	
	serverConfigFile_ = serverConfigFile;

	if (weatherConfig_ != 0)
		delete weatherConfig_;
	
	weatherConfig_ = new PlainConfigFile(WeatherGlobal::getConfigPath(serverConfigFile_));
		
	QString encoding = weatherConfig_->readEntry("Default","Encoding");
	decoder_ = QTextCodec::codecForName(encoding.ascii());
	
	host_ = weatherConfig_->readEntry("Name Search","Search host");
	httpClient_.setHost(host_);
	
	QString encodedCity = city_;
	encodeUrl(&encodedCity, encoding);
	url_.sprintf(weatherConfig_->readEntry("Name Search","Search path").ascii() , encodedCity.ascii());
	
	timerTimeout_->start(weather_global->CONNECTION_TIMEOUT, false);
	timeoutCount_ = weather_global->CONNECTION_COUNT;
	httpClient_.get(url_);
	
	kdebugf2();
}

void SearchLocationID::findNext()
{
	kdebugf();
	
	currentServer_ = weather_global->nextServer(currentServer_);
	if (currentServer_ != weather_global->endServer())
	{
		emit nextServerSearch( city_, (*currentServer_).name_);
		findNext((*currentServer_).configFile_);
	}
	else
		emit finished();
	
	kdebugf2();
}

void SearchLocationID::cancel()
{
	kdebugf();
	
	disconnect(&httpClient_, SIGNAL(finished()), this, SLOT(downloadingFinished()));
	disconnect(&httpClient_, SIGNAL(error()), this, SLOT(downloadingError()));
	disconnect(&httpClient_, SIGNAL(redirected(QString)), this, SLOT(downloadingRedirected(QString)));

	redirected_ = false;
	
	kdebugf2();
}

void SearchLocationID::downloadingFinished()
{
	kdebugf();
	
	disconnect(&httpClient_, SIGNAL(finished()), this, SLOT(downloadingFinished()));
	disconnect(&httpClient_, SIGNAL(error()), this, SLOT(downloadingError()));
	disconnect(&httpClient_, SIGNAL(redirected(QString)), this, SLOT(downloadingRedirected(QString)));
	
	timerTimeout_->stop();

	if (redirected_)
		redirected_ = false;
	else
	{
		const QByteArray &data = httpClient_.data();
		QString page = decoder_->toUnicode( data.data(), data.count());
		
		parser_.getSearch(page, weatherConfig_, serverConfigFile_, &results_);
		
		// Je�li strona zawiera wszystkie miasta,
		// trzeba dok�adniej przefiltrowa�
		if (weatherConfig_->readBoolEntry("Name Search","OnePage"))
		{
			CITYSEARCHRESULTS::iterator it, old_it;
			
			it = results_.begin();
			while (it != results_.end())
			{
				if ((*it).cityName_.find(city_, 0, false) == -1)
				{
					old_it = it;
					++it;
					results_.erase(old_it);
				}
				else
					++it;
			}
		}
	}
	
	if (searchAllServers_)
		findNext();
	else
		emit finished();
	
	kdebugf2();
}

void SearchLocationID::downloadingRedirected(QString link)
{
	kdebugf();

	QString id = parser_.getFastSearch(link, weatherConfig_);
	
	if (!id.isEmpty())
	{
		results_.append(CitySearchResult(city_, id, serverConfigFile_));
		redirected_ = true;
	}

	kdebugf2();
}

void SearchLocationID::downloadingError()
{
	kdebugf();
	
	disconnect(&httpClient_, SIGNAL(finished()), this, SLOT(downloadingFinished()));
	disconnect(&httpClient_, SIGNAL(error()), this, SLOT(downloadingError()));
	disconnect(&httpClient_, SIGNAL(redirected(QString)), this, SLOT(downloadingRedirected(QString)));

	timerTimeout_->stop();
	
	if (searchAllServers_)
		findNext();
	else
		emit error(host_ + '/' + url_);

	kdebugf2();
}

void SearchLocationID::connectionTimeout()
{
	kdebugf();
	
	if (--timeoutCount_ <= 0)
	{
		cancel();
		downloadingError();
	}
	else
	{
		httpClient_.setHost(host_);
		httpClient_.get(url_);
	}

	kdebugf2();
}

void SearchLocationID::encodeUrl(QString *str, const QString &enc) const
{
	kdebugf();
	
	if (str == 0)
		return;
	
	QUrl::encode(*str);
	
	if (enc == "ISO8859-2")
	{
		//QString rep[18][2]={{"�","%B1"},{"�","%E6"},{"�","%EA"},{"�","%B3"},{"%�","%F1"},{"�","%F3"},{"�","%B6"},{"�","%BF"},{"�","%BC"},
		//					{"�","%A1"},{"�","%C6"},{"�","%CA"},{"�","%A3"},{"�","%D1"},{"�","%D3"},{"�","%A6"},{"�","%AF"},{"�","%AC"}};
	
		QString rep[18][2]={{"%C4%85","%B1"},{"%C4%87","%E6"},{"%C4%99","%EA"},{"%C5%84","%F1"},{"%C5%82","%B3"},{"%C3%B3","%F3"},{"%C5%9B","%B6"},{"%C5%BC","%BF"},{"%C5%BA","%BC"},
							{"%C4%84","%A1"},{"%C4%86","%C6"},{"%C4%98","%CA"},{"%C5%83","%D1"},{"%C5%81","%A3"},{"%C3%93","%D3"},{"%C5%9A","%A6"},{"%C5%BB","%AF"},{"%C5%B9","%AC"}};
	
		for (int i = 0; i < 18; ++i)
		{
			str->replace(rep[i][0],rep[i][1]);
		}
	}

	kdebugf2();
}

void SearchLocationID::splitUrl(const QString &url_, QString &host, QString &path) const
{
	int endhost = url_.find('/');
	host = url_.left(endhost);
	path = url_.right(url_.length() - endhost);
}
