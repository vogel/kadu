/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include <QtCore/QDateTime>
#include <QtCore/QString>

#include "parser_extender.h"

#include "debug.h"
#include "modules.h"
#include "kadu_parser.h"
#include "config_file.h"

QDateTime started;
ParserExtender *parserExtender;

extern "C" KADU_EXPORT int parser_extender_init()
{
	kdebugf();

	parserExtender = new ParserExtender();
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/parser_extender.ui"), parserExtender);

	kdebugf2();
	return 0;
}


extern "C" KADU_EXPORT void parser_extender_close()
{
	kdebugf();

	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/parser_extender.ui"), parserExtender);
	delete parserExtender;
	parserExtender = NULL;

	kdebugf2();
}


/* 
 * Returns uptime counted from the start of the module 
 * @param mode - if 0 number of seconds is returned,
 *               if 1 formatted date is returned
 */
QString getKaduUptime(int mode)
{
	QString uptime = "0";
	
	uptime += "s ";
	if (QDateTime::currentDateTime() > started) 
	{
		int upTime = started.secsTo(QDateTime::currentDateTime());
		if (mode == 0)
		{
			uptime.setNum(upTime);
			uptime += "s ";
		}
		else 
		{
			int days = upTime/(60*60*24);
			upTime -= days * 60*60*24;
			int hours = upTime/(60*60);
			upTime -= hours * 60*60;
			int mins = upTime/(60);
			upTime -= mins * 60;
			int secs = upTime;
			
			QString s;
			uptime = s.setNum(days)+"d ";
			uptime += s.setNum(hours)+"h ";
			uptime += s.setNum(mins)+"m ";
			uptime += s.setNum(secs)+"s ";
		}
	}
	return uptime;
}

/* 
 * Returns system uptime
 * @param mode - if 0 number of seconds is returned,
 *               if 1 formatted date is returned
 */
QString getUptime(int mode)
{	
	QString uptime = "0";
	
	time_t upTime = 0;
	FILE *f;
	double duptime = 0;
	f = fopen("/proc/uptime", "r");
	fscanf(f, "%lf", &duptime);
	fclose(f);
	upTime = (time_t)duptime;
	
	QString s = "";
	if (mode == 0) {
	 	uptime = s.setNum(upTime) + "s ";
	}
	else 
	{
		int days = upTime/(60*60*24);
		upTime -= days * 60*60*24;
		int hours = upTime/(60*60);
		upTime -= hours * 60*60;
		int mins = upTime/(60);
		upTime -= mins * 60;
		int secs = upTime;
			
		uptime = s.setNum(days)+"d ";
		uptime += s.setNum(hours)+"h ";
		uptime += s.setNum(mins)+"m ";
		uptime += s.setNum(secs)+"s ";
	}
	return uptime;
}

/* Returns current time (without secs) */
QString parseTime(const UserListElement &ule)
{
    return QDateTime::currentDateTime().toString("h:mm");
}

/* Returns current time (with secs) */
QString parseLongTime(const UserListElement &ule)
{
    return QDateTime::currentDateTime().toString("hh:mm:ss");
}

/* Returns current date (without year) */
QString parseDate(const UserListElement &ule)
{
	return QDateTime::currentDateTime().toString("dd-MM");
}

/* Returns current date (with year) */
QString parseLongDate(const UserListElement &ule)
{
	return QDateTime::currentDateTime().toString("dd-MM-yyyy");
}

/* Returns time of module start (without seconds) */
QString parseStartTime(const UserListElement &ule)
{
	return started.toString("hh:mm");
}

/* Returns time of module start (with seconds) */
QString parseLongStartTime(const UserListElement &ule)
{
	return  started.toString("dd-MM-yy hh:mm:ss");
}

/* Returns uptime (seconds) */
QString parseUptime(const UserListElement &ule)
{
	return getUptime(0);
}

/* Returns uptime (formatted) */
QString parseLongUptime(const UserListElement &ule)
{
	return getUptime(1);
}

/* Returns Kadu uptime */
QString parseKaduUptime(const UserListElement &ule) 
{
	return getKaduUptime(0);
}

/* Returns Kadu uptime (formatted) */
QString parseLongKaduUptime(const UserListElement &ule)
{
	return getKaduUptime(1);
}

ParserExtender::ParserExtender()
{
	if (config_file.readEntry("PowerKadu", "enable_parser_extender") == "true") {
		init();
		isStarted = true;
	}
	else 
		isStarted = false;
}

ParserExtender::~ParserExtender()
{
	if (config_file.readEntry("PowerKadu", "enable_parser_extender") == "true")
		close();
}

void ParserExtender::init()
{
	/* store the date of module start */
	started = QDateTime::currentDateTime();

	/* register tags */	
	KaduParser::registerTag("time", &parseTime);
	KaduParser::registerTag("time-long", &parseLongTime);
	KaduParser::registerTag("date", &parseDate);
	KaduParser::registerTag("date-long", &parseLongDate);
	KaduParser::registerTag("start", &parseStartTime);
	KaduParser::registerTag("start-long", &parseLongStartTime);
	KaduParser::registerTag("uptime", &parseUptime);
	KaduParser::registerTag("uptime-long", &parseLongUptime);
	KaduParser::registerTag("kuptime", &parseKaduUptime);
	KaduParser::registerTag("kuptime-long", &parseLongKaduUptime);
}

void ParserExtender::close()
{
	/* unregister tags */
	KaduParser::unregisterTag("time", &parseTime);
	KaduParser::unregisterTag("time-long", &parseLongTime);
	KaduParser::unregisterTag("date", &parseDate);
	KaduParser::unregisterTag("date-long", &parseLongDate);
	KaduParser::unregisterTag("start", &parseStartTime);
	KaduParser::unregisterTag("start-long", &parseLongStartTime);
	KaduParser::unregisterTag("uptime", &parseUptime);
	KaduParser::unregisterTag("uptime-long", &parseLongUptime);
	KaduParser::unregisterTag("kuptime", &parseKaduUptime);
	KaduParser::unregisterTag("kuptime-long", &parseLongKaduUptime);
}

void ParserExtender::configurationUpdated()
{
	if ((config_file.readEntry("PowerKadu", "enable_parser_extender") == "false") && isStarted) {
		close();
		isStarted = false;
	}
	else if ((config_file.readEntry("PowerKadu", "enable_parser_extender") == "true") && !isStarted) {
		init();
		isStarted = true;
	}
}

void ParserExtender::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
}
