/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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
				ret[0] = ret.at(0).toUpper(); // looks ugly lowercase ;)
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

