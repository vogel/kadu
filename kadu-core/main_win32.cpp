/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include <QtCore/QLibrary>
#include <QtGui/QApplication>
#include <QtGui/QMessageBox>

#include "configuration/configuration-file.h"
#include "core/core.h"
#include "misc/misc.h"

#include <windows.h>
#ifdef _MSC_VER
#include <dbghelp.h>
#endif
#include "configuration/xml-configuration-file.h"
#include "core/crash-aware-object.h"

#ifdef _MSC_VER
typedef BOOL (WINAPI *MiniDumpWriteDump_t)(HANDLE hProcess, DWORD ProcessId, HANDLE hFile, MINIDUMP_TYPE DumpType,
		PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
		PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
		PMINIDUMP_CALLBACK_INFORMATION CallbackParam);

MiniDumpWriteDump_t MiniDumpWriteDump_f;
#endif

LONG WINAPI exception_handler(struct _EXCEPTION_POINTERS *e)
{
	Q_UNUSED(e)

	CrashAwareObject::notifyCrash();

#ifdef _MSC_VER
	LONG ret = EXCEPTION_CONTINUE_SEARCH;
	if (MiniDumpWriteDump_f)
	{
		// use plain winapi to prevent further mess
		WCHAR filename[MAX_PATH];
		WCHAR temp[MAX_PATH];
		SYSTEMTIME time;
		GetTempPathW(MAX_PATH, temp);
		GetSystemTime(&time);
		_snwprintf(filename, MAX_PATH, L"%s\\Kadu-%S-%04d-%02d-%02dT%02d%02d%02d.dmp",
				temp, qPrintable(Core::version()), time.wYear, time.wMonth, time.wDay,
				time.wHour, time.wMinute, time.wSecond);
		HANDLE handle = CreateFileW(filename, GENERIC_READ | GENERIC_WRITE, 0, NULL,
				CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (handle != NULL && handle != INVALID_HANDLE_VALUE)
		{
			MINIDUMP_EXCEPTION_INFORMATION mdei;
			mdei.ThreadId = GetCurrentThreadId();
			mdei.ExceptionPointers  = e;
			mdei.ClientPointers     = FALSE;

			BOOL result = MiniDumpWriteDump_f(GetCurrentProcess(), GetCurrentProcessId(),
				handle, MiniDumpNormal, (e != 0) ? &mdei : NULL, 0, 0);

			if (result)
			{
				WCHAR msg[MAX_PATH + 128];
				_snwprintf(msg, MAX_PATH + 128, L"Mini dump written to file `%s'.\nUse Ctrl+C to copy this message.", filename);
				MessageBoxW(NULL, msg, L"Kadu crashed", MB_OK | MB_ICONINFORMATION);
				ret = EXCEPTION_EXECUTE_HANDLER;
			}
			else
				MessageBoxW(NULL, L"Unable to write mini dump.", L"Kadu crashed", MB_OK | MB_ICONERROR);
			CloseHandle(handle);
		}
	}

	// if we cannot make crash dump only save config file to backup
	QString f = QString("kadu.conf.xml.backup.%1").arg(QDateTime::currentDateTime().toString("yyyy.MM.dd.hh.mm.ss"));
	xml_config_file->saveTo(KaduPaths::instance()->profilePath() + f);
	return ret;
#else
	MessageBoxW(NULL, L"Mini dumps are not available in this build.", L"Kadu crashed", MB_OK | MB_ICONERROR);
	return EXCEPTION_EXECUTE_HANDLER;
#endif /* _MSC_VER */
}

void enableSignalHandling()
{
#ifdef _MSC_VER
	MiniDumpWriteDump_f = (MiniDumpWriteDump_t)QLibrary::resolve("dbghelp", "MiniDumpWriteDump");
//	SetUnhandledExceptionFilter(exception_handler);
#endif /* _MSC_VER */
}
