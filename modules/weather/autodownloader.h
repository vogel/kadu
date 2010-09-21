/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef AUTODOWNLOADER_H
#define AUTODOWNLOADER_H

#include <memory>
#include <QtCore/QObject>

#include "configuration_aware_object.h"
#include "status_changer.h"

#include "get_forecast.h"


class QTimer;

class WeatherStatusChanger : public StatusChanger
{
	Q_OBJECT
public:
	WeatherStatusChanger();
	void setDescription( const QString& description );
	void setEnabled( bool enabled );
	bool enabled() const					{ return enabled_; }

private:
	virtual void changeStatus( UserStatus& status );
	
private:
	QString description_;
	bool enabled_;
};

class AutoDownloader : public QObject, ConfigurationAwareObject
{
	Q_OBJECT
	
public:
	AutoDownloader();
	~AutoDownloader();
	
private:
	QTimer* autoDownloadTimer_;
	std::auto_ptr<GetForecast> downloader_;
	std::auto_ptr<WeatherStatusChanger> statusChanger_;
	bool fetchingEnabled_;
	bool hintsEnabled_;
	bool descEnabled_;
	
	virtual void configurationUpdated();
	QString parse(const ForecastDay& day, QString str);

private slots:
	void autoDownload();
	void autoDownloadingFinished();
	
};

#endif // AUTODOWNLOADER_H
