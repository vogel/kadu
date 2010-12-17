/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <algorithm>

#include "misc.h"
#include "config_file.h"

#include "weather_global.h"

const int WeatherGlobal::CONNECTION_TIMEOUT = 15000;
const int WeatherGlobal::CONNECTION_COUNT = 2;
const unsigned int WeatherGlobal::RECENT_LOCATIONS_COUNT = 10;
const int WeatherGlobal::KEEP_FORECAST = 1; // w godzinach

namespace
{
	struct HasName
	{
		bool operator() (const WeatherGlobal::Server &server)
		{
			return (server.name_ == name_);
		}

		HasName(const QString &name) : name_(name) {}
		QString name_;
	};

	struct HasConfig
	{
		bool operator() (const WeatherGlobal::Server &server)
		{
			return (server.configFile_ == configFile_);
		}

		HasConfig(const QString &configFile ) : configFile_(configFile) {}
		QString configFile_;
	};

	const QString WeatherConfigPath("kadu/modules/data/weather");
	const QString WeatherIconPath("kadu/modules/data/weather/icons");
} // namespace

WeatherGlobal::WeatherGlobal()
{
	QStringList serverList = QStringList::split(";", config_file.readEntry("Weather", "Servers"));
	QStringList serversUsing = QStringList::split(";", config_file.readEntry("Weather", "ServersUsing"));

	QDir dir(dataPath(WeatherConfigPath), "*.ini");
	dir.setFilter(QDir::Files);

	QStringList iniFiles;
	for (unsigned int i = 0; i < dir.count(); ++i)
		iniFiles.append(dir[i]);

	// Remove non-existent files from the server list
	//
	for (unsigned int i = 0; i < serverList.count(); ++i)
	{
		QStringList::iterator file = iniFiles.find(serverList[i]);
		if (file == iniFiles.end())
		{
			serverList.removeAt(i);
			serversUsing.removeAt(i);
		}
		else
			iniFiles.erase(file);
	}

	// Add new files to the server list
	//
	serverList += iniFiles;
	for (unsigned int i = 0; i < iniFiles.count(); ++i)
		serversUsing.append("1");

	// Load server configs and initialize server list
	//
	for (unsigned int i = 0; i < serverList.count(); ++i)
	{
		PlainConfigFile wConfig(getConfigPath(serverList[i]));
		servers_.append(Server(wConfig.readEntry("Header","Name"), serverList[i], serversUsing[i] == "1"));
	}

	// Load recent locations search list
	//
	for (unsigned int i = 0; i < RECENT_LOCATIONS_COUNT; i++)
	{
		QString city = config_file.readEntry("Weather", QString("Location%1").arg(i + 1));
		if (!city.isEmpty() && !recentLocations_.contains(city))
			recentLocations_.push_back(city);
	}

}

WeatherGlobal::~WeatherGlobal()
{
	unsigned int i;
	for (i = 0; i < recentLocations_.count(); i++)
		config_file.writeEntry("Weather", QString("Location%1").arg(i + 1), recentLocations_[i]);
	for (; i < RECENT_LOCATIONS_COUNT; i++)
		config_file.writeEntry("Weather", QString("Location%1").arg(i + 1), "");


	QStringList serverList;
	QStringList serversUsing;

	SERVERITERATOR server = servers_.begin();
	while (server != servers_.end())
	{
		serverList.append((*server).configFile_);
		serversUsing.append((*server).use_ ? "1" : "0");
		++server;
	}

	config_file.writeEntry("Weather", "Servers", serverList.join(";"));
	config_file.writeEntry("Weather", "ServersUsing", serversUsing.join(";"));
}

WeatherGlobal::SERVERITERATOR WeatherGlobal::beginServer() const
{
	SERVERITERATOR it = servers_.begin();
	while (it != servers_.end())
	{
		if ((*it).use_)
			return it;
		else
			++it;
	}
	return servers_.end();
}

WeatherGlobal::SERVERITERATOR WeatherGlobal::nextServer(SERVERITERATOR it) const
{
	++it;
	while (it != servers_.end())
	{
		if ((*it).use_)
			return it;
		else
			++it;
	}
	return servers_.end();
}

QString WeatherGlobal::getConfigFile(const QString &serverName) const
{
	SERVERS::const_iterator server = std::find_if(servers_.begin(), servers_.end(), HasName(serverName));
	if (server != servers_.end())
		return server->configFile_;
	else
		QString();
}

QString WeatherGlobal::getServerName(const QString &configFile) const
{
	SERVERS::const_iterator server = std::find_if(servers_.begin(), servers_.end(), HasConfig(configFile));
	if (server != servers_.end())
		return server->name_;
	else
		QString();
}

bool WeatherGlobal::configFileExists(const QString &configFile) const
{
	SERVERS::const_iterator server = std::find_if(servers_.begin(), servers_.end(), HasConfig(configFile));
	return (server != servers_.end());
}

void WeatherGlobal::setServerUsing(const QString serverName, bool use)
{
	SERVERS::iterator server = std::find_if(servers_.begin(), servers_.end(), HasName(serverName));
	if (server != servers_.end())
		server->use_ = use;
}

void WeatherGlobal::setServerPos(const QString &serverName, uint pos)
{
	uint i = 0;
	for (SERVERS::iterator it = servers_.begin(); it != servers_.end(); ++it, ++i)
	{
		const Server &server = *it;
		if (server.name_ == serverName)
		{
			if (i != pos)
				servers_.insert(pos, servers_.takeAt(i));
			return;
		}
	}
}

bool WeatherGlobal::insertRecentLocation(const QString &location)
{
	if (recentLocations_.find(location) == recentLocations_.end())
	{
		recentLocations_.push_front(location);
		if (recentLocations_.count() > RECENT_LOCATIONS_COUNT)
			recentLocations_.remove(recentLocations_.at(RECENT_LOCATIONS_COUNT));

		return true;
	}
	else
		return false;
}

QString WeatherGlobal::getConfigPath(const QString &file)
{
	return dataPath(WeatherConfigPath) + '/' + file;
}

QString WeatherGlobal::getIconPath(const QString &file)
{
	return dataPath(WeatherIconPath) + '/' + file;
}
