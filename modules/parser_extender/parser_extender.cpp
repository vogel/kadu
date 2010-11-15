/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */


#include <QtCore/QDateTime>
#include <QtCore/QString>

#include <stdio.h>

#include "configuration/configuration-file.h"
#include "gui/windows/main-configuration-window.h"
#include "misc/path-conversion.h"
#include "parser/parser.h"

#include "debug.h"
#include "modules.h"

#include "parser_extender.h"

QDateTime started;
ParserExtender *parserExtender;

extern "C" KADU_EXPORT int parser_extender_init()
{
	kdebugf();

	parserExtender = new ParserExtender();
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/parser_extender.ui"));

	kdebugf2();
	return 0;
}


extern "C" KADU_EXPORT void parser_extender_close()
{
	kdebugf();

	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/parser_extender.ui"));
	delete parserExtender;
	parserExtender = 0;

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

#ifdef Q_OS_LINUX //TODO 0.6.6: find more portable way to get system uptime
	time_t upTime = 0;
	FILE *f;
	double duptime = 0;
	f = fopen("/proc/uptime", "r");
	if (!fscanf(f, "%lf", &duptime))
	{
		fclose(f);
		return QString::null;
	}

	fclose(f);
	upTime = (time_t)duptime;

	QString s = "";
	if (mode == 0)
	 	uptime = s.setNum(upTime) + "s ";
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
#else
	Q_UNUSED(mode)
#endif

	return uptime;
}

/* Returns current time (without secs) */
QString parseTime(BuddyOrContact buddyOrContact)
{
	Q_UNUSED(buddyOrContact)

	return QDateTime::currentDateTime().toString("h:mm");
}

/* Returns current time (with secs) */
QString parseLongTime(BuddyOrContact buddyOrContact)
{
	Q_UNUSED(buddyOrContact)

	return QDateTime::currentDateTime().toString("hh:mm:ss");
}

/* Returns current date (without year) */
QString parseDate(BuddyOrContact buddyOrContact)
{
	Q_UNUSED(buddyOrContact)

	return QDateTime::currentDateTime().toString("dd-MM");
}

/* Returns current date (with year) */
QString parseLongDate(BuddyOrContact buddyOrContact)
{
	Q_UNUSED(buddyOrContact)

	return QDateTime::currentDateTime().toString("dd-MM-yyyy");
}

/* Returns time of module start (without seconds) */
QString parseStartTime(BuddyOrContact buddyOrContact)
{
	Q_UNUSED(buddyOrContact)

	return started.toString("hh:mm");
}

/* Returns time of module start (with seconds) */
QString parseLongStartTime(BuddyOrContact buddyOrContact)
{
	Q_UNUSED(buddyOrContact)

	return  started.toString("dd-MM-yy hh:mm:ss");
}

/* Returns uptime (seconds) */
QString parseUptime(BuddyOrContact buddyOrContact)
{
	Q_UNUSED(buddyOrContact)

	return getUptime(0);
}

/* Returns uptime (formatted) */
QString parseLongUptime(BuddyOrContact buddyOrContact)
{
	Q_UNUSED(buddyOrContact)

	return getUptime(1);
}

/* Returns Kadu uptime */
QString parseKaduUptime(BuddyOrContact buddyOrContact)
{
	Q_UNUSED(buddyOrContact)

	return getKaduUptime(0);
}

/* Returns Kadu uptime (formatted) */
QString parseLongKaduUptime(BuddyOrContact buddyOrContact)
{
	Q_UNUSED(buddyOrContact)

	return getKaduUptime(1);
}

ParserExtender::ParserExtender()
{
	if (config_file.readEntry("PowerKadu", "enable_parser_extender") == "true")
	{
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
	Parser::registerTag("time", &parseTime);
	Parser::registerTag("time-long", &parseLongTime);
	Parser::registerTag("date", &parseDate);
	Parser::registerTag("date-long", &parseLongDate);
	Parser::registerTag("start", &parseStartTime);
	Parser::registerTag("start-long", &parseLongStartTime);
	Parser::registerTag("uptime", &parseUptime);
	Parser::registerTag("uptime-long", &parseLongUptime);
	Parser::registerTag("kuptime", &parseKaduUptime);
	Parser::registerTag("kuptime-long", &parseLongKaduUptime);
}

void ParserExtender::close()
{
	/* unregister tags */
	Parser::unregisterTag("time", &parseTime);
	Parser::unregisterTag("time-long", &parseLongTime);
	Parser::unregisterTag("date", &parseDate);
	Parser::unregisterTag("date-long", &parseLongDate);
	Parser::unregisterTag("start", &parseStartTime);
	Parser::unregisterTag("start-long", &parseLongStartTime);
	Parser::unregisterTag("uptime", &parseUptime);
	Parser::unregisterTag("uptime-long", &parseLongUptime);
	Parser::unregisterTag("kuptime", &parseKaduUptime);
	Parser::unregisterTag("kuptime-long", &parseLongKaduUptime);
}

void ParserExtender::configurationUpdated()
{
	if ((config_file.readEntry("PowerKadu", "enable_parser_extender") == "false") && isStarted)
	{
		close();
		isStarted = false;
	}
	else if ((config_file.readEntry("PowerKadu", "enable_parser_extender") == "true") && !isStarted)
	{
		init();
		isStarted = true;
	}
}
