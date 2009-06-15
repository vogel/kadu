/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QDateTime>
#include <QtGui/QApplication>

#ifdef Q_OS_WIN
#include <sys/timeb.h>
#else
#include <sys/time.h>
#endif
#include <time.h>

#include "configuration/configuration-file.h"

#include "date-time.h"

QString printDateTime(const QDateTime &datetime)
{
	QString ret;
	QDateTime current_date;
	unsigned int delta;

	current_date.setTime_t(time(NULL));
//	current_date.setTime(QTime(0, 0));

	delta = datetime.daysTo(current_date);
	ret = datetime.toString("hh:mm:ss");

	if (delta != 0)
	{
		if (config_file.readBoolEntry("Look", "NiceDateFormat"))
		{
			if (delta == 1) // 1 day ago
				ret.prepend(qApp->translate("@default", "Yesterday at "));
			else if (delta < 7) // less than week ago
			{
				ret.prepend(datetime.toString(qApp->translate("@default", "dddd at ")));
				ret[0] = ret[0].toUpper(); // looks ugly lowercase ;)
			}
			else if ((delta > 7) && (delta < 14))
			{
				int tmp = delta % 7;
				if (tmp == 0)
					ret.prepend(qApp->translate("@default", "week ago at "));
				else if (tmp == 1)
					ret.prepend(qApp->translate("@default", "week and day ago at "));
				else
					ret.prepend(qApp->translate("@default", "week and %2 days ago at ").arg(delta%7));
			}
			else if (delta < 6*7)
			{
				int tmp = delta % 7;
				if (tmp == 0)
					ret.prepend(qApp->translate("@default", "%1 weeks ago at ").arg(delta/7));
				else if (tmp == 1)
					ret.prepend(qApp->translate("@default", "%1 weeks and day ago at ").arg(delta/7));
				else
					ret.prepend(qApp->translate("@default", "%1 weeks and %2 days ago at ").arg(delta/7).arg(delta%7));
			}
			else
				ret.prepend(datetime.toString(qApp->translate("@default", "d MMMM yyyy at ")));
		}
		else
			ret.append(datetime.toString(" (dd.MM.yyyy)"));
	}
	return ret;
}

QString timestamp(time_t customtime)
{
	QString buf;
	QDateTime date;
	time_t t;

	t = time(NULL);

	date.setTime_t(t);
	buf.append(printDateTime(date));

	if (customtime)
	{
		date.setTime_t(customtime);
		buf.append(QString(" / S ") + printDateTime(date));
	}

	return buf;
}

QDateTime currentDateTime(void)
{
	time_t t;
	QDateTime date;

	t = time(NULL);
	date.setTime_t(t);
	return date;
}

void getTime(time_t *sec, int *msec)
{
#ifdef Q_OS_WIN
	struct _timeb timebuffer;

	_ftime(&timebuffer);

	*sec = timebuffer.time;
	*msec = timebuffer.millitm * 1000;
#else
	struct timeval tv;
	struct timezone tz;

	gettimeofday(&tv, &tz);

	*sec=tv.tv_sec;
	*msec=tv.tv_usec * 1000;
#endif
}

