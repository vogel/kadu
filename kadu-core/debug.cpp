/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2008, 2009, 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <stdarg.h>
#include <stdio.h>

#include "misc/date-time.h"
#include "debug.h"

int debug_mask = -2;

#ifdef DEBUG_OUTPUT_ENABLED

#include <QtCore/QDebug>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>

#ifdef Q_OS_WIN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

bool KADUAPI showTimesInDebug = false;

void _kdebug_with_mask(int mask, const char* file, const int line, const char* format,...)
{
	static long last;
	static QMutex mutex;

	if (debug_mask & mask)
	{
		QMutexLocker locker(&mutex);

		if (showTimesInDebug)
		{
			time_t sec;
			long int msec;
			getTime(&sec, &msec);
			long x = (sec % 1000) * 1000 + msec;
			fprintf(stderr, "KK <%d:%03ld:%09ld:%s:%i>\t", (int)sec, msec, x - last, file, line);
			last = x;
		}
		else
			fprintf(stderr, "KK <%s:%i>\t", file, line);

#ifdef Q_OS_WIN
		HANDLE hConsole=GetStdHandle(STD_ERROR_HANDLE);
		// read old attributes
		CONSOLE_SCREEN_BUFFER_INFO ci;
		GetConsoleScreenBufferInfo(hConsole, &ci);
		if (mask & KDEBUG_WARNING)
			SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE);//niebieski
		else if (mask & KDEBUG_ERROR)
			SetConsoleTextAttribute(hConsole, FOREGROUND_RED);//���ty
		else if (mask & KDEBUG_PANIC)
			SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);//jasny czerwony
#else
		if (mask & KDEBUG_WARNING)
			fprintf(stderr, "\033[34m");//niebieski
		else if (mask & KDEBUG_ERROR)
			fprintf(stderr, "\033[33;1m");//���ty
		else if (mask & KDEBUG_PANIC)
			fprintf(stderr, "\033[31;1m");//jasny czerwony
#endif

		va_list args;
		va_start(args, format);
		vfprintf(stderr, format, args);
		va_end(args);
		if (mask & (KDEBUG_PANIC|KDEBUG_ERROR|KDEBUG_WARNING))
#ifdef Q_OS_WIN
			SetConsoleTextAttribute(hConsole, ci.wAttributes);
#else
			fprintf(stderr, "\033[0m");
#endif
		fflush(stderr);
#ifdef Q_OS_WIN
		// ok, now send it to debugger without times
		char msg[1024];
		if(IsDebuggerPresent()){
			_snprintf(msg, 1024, "KK <%s:%i> ", file, line);
			OutputDebugString(msg);
			va_start(args, format);
			vsnprintf(msg, 1021, format, args);
			va_end(args);
			OutputDebugString(msg);
		}
#endif
	}
}

#endif


namespace Debug
{

void ktDebugStart(const QString &message, QTime &time)
{
	time = QTime::currentTime();

	printf("%s", qPrintable(QString("[timer start] %1\n").arg(message)));
}

void ktDebugCheckPoint(const QString& message, QTime& time)
{
	time = QTime::currentTime();

	printf("%s", qPrintable(QString("[timer checkpoint: %1] %2\n").arg(QTime::currentTime().msecsTo(time)).arg(message)));
}

} // namespace Debug
