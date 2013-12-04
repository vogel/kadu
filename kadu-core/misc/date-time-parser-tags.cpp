/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * %kadu copyright end%
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
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

#include "parser/parser.h"

#include "date-time-parser-tags.h"

static QDateTime started;

/**
 * Returns Kadu uptime.
 * @param formatted If false number of seconds is returned,
 *                  otherwise formatted date.
 */
static QString getKaduUptime(bool formatted)
{
	QString uptime("0 s ");

	if (QDateTime::currentDateTime() > started)
	{
		int upTime = started.secsTo(QDateTime::currentDateTime());
		if (!formatted)
			uptime = QString::number(upTime) + "s ";
		else
		{
			int days = upTime / (60 * 60 * 24);
			upTime -= days * (60 * 60 * 24);
			int hours = upTime / (60 * 60);
			upTime -= hours * (60 * 60);
			int mins = upTime / 60;
			upTime -= mins * 60;
			int secs = upTime;

			uptime = QString::number(days) + "d " +
					QString::number(hours) + "h " +
					QString::number(mins) + "m " +
					QString::number(secs) + "s ";
		}
	}

	return uptime;
}

/**
 * Returns system uptime.
 * @param formatted If false, number of seconds is returned,
 *                  otherwise formatted date.
 * @note Currently works only on Linux.
 * @todo Find more portable way to get system uptime.
 */
static QString getUptime(bool formatted)
{
	QString uptime("0 s ");

#ifdef Q_OS_LINUX
	FILE *f;
	if (!(f = fopen("/proc/uptime", "r")))
		return uptime;

	double duptime = 0.;
	int ret = fscanf(f, "%lf", &duptime);
	fclose(f);

	if (ret == EOF || ret == 0)
		return uptime;

	time_t upTime = (time_t)duptime;

	if (!formatted)
	 	uptime = QString::number(upTime) + "s ";
	else
	{
		time_t days = upTime / (60 * 60 * 24);
		upTime -= days * (60 * 60 * 24);
		time_t hours = upTime / (60 * 60);
		upTime -= hours * (60 * 60);
		time_t mins = upTime / 60;
		upTime -= mins * 60;
		time_t secs = upTime;

		uptime = QString::number(days) + "d " +
				QString::number(hours) + "h " +
				QString::number(mins) + "m " +
				QString::number(secs) + "s ";
	}
#else
	Q_UNUSED(formatted)
#endif

	return uptime;
}

/** Returns current time (without secs) */
static QString parseTime(Talkable talkable)
{
	Q_UNUSED(talkable)

	return QDateTime::currentDateTime().toString("h:mm");
}

/** Returns current time (with secs) */
static QString parseLongTime(Talkable talkable)
{
	Q_UNUSED(talkable)

	return QDateTime::currentDateTime().toString("hh:mm:ss");
}

/** Returns current date (without year) */
static QString parseDate(Talkable talkable)
{
	Q_UNUSED(talkable)

	return QDateTime::currentDateTime().toString("dd-MM");
}

/** Returns current date (with year) */
static QString parseLongDate(Talkable talkable)
{
	Q_UNUSED(talkable)

	return QDateTime::currentDateTime().toString("dd-MM-yyyy");
}

/** Returns time of Kadu start (without seconds) */
static QString parseStartTime(Talkable talkable)
{
	Q_UNUSED(talkable)

	return started.toString("hh:mm");
}

/** Returns time of Kadu start (with seconds) */
static QString parseLongStartTime(Talkable talkable)
{
	Q_UNUSED(talkable)

	return started.toString("dd-MM-yy hh:mm:ss");
}

/** Returns system uptime (seconds) */
static QString parseUptime(Talkable talkable)
{
	Q_UNUSED(talkable)

	return getUptime(false);
}

/** Returns system uptime (formatted) */
static QString parseLongUptime(Talkable talkable)
{
	Q_UNUSED(talkable)

	return getUptime(true);
}

/** Returns Kadu uptime */
static QString parseKaduUptime(Talkable talkable)
{
	Q_UNUSED(talkable)

	return getKaduUptime(false);
}

/** Returns Kadu uptime (formatted) */
static QString parseLongKaduUptime(Talkable talkable)
{
	Q_UNUSED(talkable)

	return getKaduUptime(true);
}

void DateTimeParserTags::registerParserTags()
{
	// TODO: do it pretty, possibly in Core
	started = QDateTime::currentDateTime();

	Parser::registerTag("time", parseTime);
	Parser::registerTag("time-long", parseLongTime);
	Parser::registerTag("date", parseDate);
	Parser::registerTag("date-long", parseLongDate);
	Parser::registerTag("start", parseStartTime);
	Parser::registerTag("start-long", parseLongStartTime);
	Parser::registerTag("uptime", parseUptime);
	Parser::registerTag("uptime-long", parseLongUptime);
	Parser::registerTag("kuptime", parseKaduUptime);
	Parser::registerTag("kuptime-long", parseLongKaduUptime);
}
