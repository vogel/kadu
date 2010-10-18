/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef WEATHER_GLOBAL_H
#define WEATHER_GLOBAL_H

#include <QtCore/QList>
#include <QtCore/QStringList>

#include "forecast_container.h"

struct WeatherGlobal
{
	WeatherGlobal();
	~WeatherGlobal();
	
	struct Server
	{
		QString name_;
		QString configFile_;
		bool use_;
		
		Server() : use_(true) {}
		Server(const Server &s) : name_(s.name_), configFile_(s.configFile_), use_(s.use_) {}
		Server(const QString &name, const QString& configFile, bool use) : name_(name), configFile_(configFile), use_(use) {}
	};
	/**
		klucz - nazwa serwisu pogodowego
		wartosc - nazwa pliku
	**/
	typedef QList<Server> SERVERS;
	typedef SERVERS::const_iterator SERVERITERATOR;
	SERVERS servers_;
	
	ForecastContainer savedForecast_;

	QStringList recentLocations_;
	
	static const int CONNECTION_TIMEOUT;
	static const int CONNECTION_COUNT;
	static const unsigned int RECENT_LOCATIONS_COUNT;
	static const int KEEP_FORECAST; // w godzinach
	
	SERVERITERATOR beginServer() const;
	SERVERITERATOR nextServer(SERVERITERATOR it) const;
	SERVERITERATOR endServer() const {return servers_.end();}

	/**
		\fn getConfigFile
		\return plik konfiguracyjny serwisu pogodowego o podanej nazwie
	**/
	QString getConfigFile(const QString &serverName) const;
	
	/**
		\fn getServerName
		\return nazwa serwisu dla podanego pliku konfiguracyjnego serwisu
	**/
	QString getServerName(const QString &configFile) const;
	
	/**
		\fn configFileExists
		Sprawdza, czy istnieje plik konfiguracyjny serwsu pogodowego
		\return true, jesli plik istnieje
	**/
	bool configFileExists(const QString &configFile) const;
	
	/**
		\fn setServerUsing
		Ustawia serwis pogodowy jako uzywany/nieu�ywany
		\param severName nazwa serwisu
		\param use true, jesli ma byc uzywany, false, jesli nie
	**/
	void setServerUsing(const QString serverName, bool use);

	void setServerPos(const QString &serverName, uint pos);

	/**
		Dodaje pozycje na liste ostatnio szukanych miejscowosci
		\param location nazwa miejscowosci
		\return true, jesli 'location' bylo nowe, false, jesli juz bylo na liscie
	**/
	bool insertRecentLocation(const QString &location);
	
	static QString getConfigPath(const QString &file);
	
	static QString getIconPath(const QString &file);
	
};

extern WeatherGlobal *weather_global;

#endif // WEATHER_GLOBAL_H
