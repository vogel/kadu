/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GET_FORECAST_H
#define GET_FORECAST_H

#include "http_client.h"

#include "forecast.h"
#include "weather_parser.h"

/**
	\class GetForecast
	Zajmuje siê pobraniem strony WWW oraz wydobyciem z niej
	informacji pogodowych.
**/
class GetForecast : public QObject
{
	Q_OBJECT
	
	private:
		QString host_;
		QString url_;
		HttpClient httpClient_;
		QTextCodec* decoder_;
		
		Forecast forecast_;
		WeatherParser parser_;
		
		QTimer* timerTimeout_;
		int timeoutCount_;
		
		PlainConfigFile* wConfig_;
		void splitUrl(const QString& url, QString& host, QString& path) const;
	
	public:
		GetForecast();
		~GetForecast();

		/**
			Pobiera prognoze.
			Po zakoñczeniu wywo³ywany jest sygna³ \see finished() lub \see error()
		**/
		void downloadForecast(const QString& configFile, const QString& locID);

		const Forecast& getForecast() { return forecast_; }
		
		enum ErrorId
		{
			Parser,
			Connection
		};
	
	public slots:
		void downloadingFinished();
		void downloadingRedirected(QString link);
		void downloadingError();
		void connectionTimeout();
	
	signals:
		void finished();
		void error( GetForecast::ErrorId err, QString url );
};

#endif // GET_FORECAST_H
