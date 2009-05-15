/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QDateTime>
#include <QtCore/QLibrary>
#include <QtGui/QApplication>
#include <QtGui/QMessageBox>

#include "config_file.h"
#include "kadu.h"
#include "misc.h"

#include <windows.h>
#ifndef _MSC_VER 
# include "dbghelp2.h" /* MinGW's DbgHelp.h lacks some definitions */
#else
# include <dbghelp.h>
#endif /* _MSC_VER */
#include "kadu-config.h"

typedef BOOL (WINAPI *MiniDumpWriteDump_t)(HANDLE hProcess, DWORD ProcessId, HANDLE hFile, MINIDUMP_TYPE DumpType,
		PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
		PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
		PMINIDUMP_CALLBACK_INFORMATION CallbackParam);

MiniDumpWriteDump_t MiniDumpWriteDump_f;
HANDLE mutex;

#define WM_OPEN_CHAT WM_USER+1

class KaduMessageWindow : public QWidget
{
	virtual bool winEvent (MSG * message, long * result){
		switch(message->message){
			case WM_OPEN_CHAT:
				qApp->postEvent(kadu, new OpenGGChatEvent(message->wParam));
				return false;
				break;
		}
		return QWidget::winEvent(message, result);
	}

	public:
		KaduMessageWindow() {
			setWindowTitle("kadu_message_window");
		}
};

LONG WINAPI exception_handler(struct _EXCEPTION_POINTERS* e)
{
	LONG ret=EXCEPTION_CONTINUE_SEARCH;
	if(MiniDumpWriteDump_f)
	{
		// use plain winapi to prevent further mess 
		WCHAR filename[MAX_PATH];
		WCHAR temp[MAX_PATH];
		SYSTEMTIME time;
		GetTempPathW(MAX_PATH, temp);
		GetSystemTime(&time);
		_snwprintf(filename, MAX_PATH, L"%s\\Kadu-%S-%04d-%02d-%02dT%02d%02d%02d.dmp",
			temp, VERSION, time.wYear, time.wMonth, time.wDay,
			time.wHour, time.wMinute, time.wSecond);
		HANDLE handle=CreateFileW(filename, GENERIC_READ | GENERIC_WRITE, 0, NULL,
		       CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
		if(handle!=NULL && handle!=INVALID_HANDLE_VALUE)
		{
			MINIDUMP_EXCEPTION_INFORMATION mdei;
			mdei.ThreadId = GetCurrentThreadId();
			mdei.ExceptionPointers  = e; 
			mdei.ClientPointers     = FALSE; 
		
			BOOL result = MiniDumpWriteDump_f(GetCurrentProcess(), GetCurrentProcessId(),
				handle, MiniDumpNormal, (e != 0) ? &mdei : NULL, 0, 0 ); 

			if(result){
				_snwprintf(temp, MAX_PATH, L"Mini dump written to %s\nUse Ctrl+C to copy this message", filename);
				MessageBoxW(NULL, temp, L"Kadu crash", MB_OK | MB_ICONINFORMATION);
				ret=EXCEPTION_EXECUTE_HANDLER;
			}
			else {
				MessageBoxW(NULL, L"Unable to write mini dump", L"Kadu", MB_OK | MB_ICONERROR);
			}
			CloseHandle(handle);		
		}
	}

	// if we cannot make crash dump only save config file to backup
	QString f = QString("kadu.conf.xml.backup.%1").arg(QDateTime::currentDateTime().toString("yyyy.MM.dd.hh.mm.ss"));
	xml_config_file->saveTo(ggPath(f));
	return ret;
}


void enableSignalHandling()
{
	char *t=getenv("DISABLE_DUMPS");
	if(!t){
		MiniDumpWriteDump_f=(MiniDumpWriteDump_t)QLibrary::resolve("dbghelp", "MiniDumpWriteDump");
		SetUnhandledExceptionFilter(exception_handler);
	}
}

bool isRuning(int ggnumber)
{
	QString uid=QApplication::applicationFilePath().toLower().replace("/", "_");

	mutex = CreateMutexW(NULL, TRUE, (WCHAR*)uid.utf16());
	if (mutex && GetLastError()==ERROR_ALREADY_EXISTS){
		CloseHandle(mutex);
		if(ggnumber){
			HWND hwnd=FindWindow("QWidget", "kadu_message_window");
			if(hwnd)
				SendMessage(hwnd, WM_OPEN_CHAT, ggnumber, 0);
			return true;
		}
		else if (QMessageBox::warning(NULL, "Kadu",
		qApp->translate("@default", QT_TR_NOOP("Another Kadu is running on this profile but I cannot get its process ID.")),
		qApp->translate("@default", QT_TR_NOOP("Force running Kadu (not recommended).")),
		qApp->translate("@default", QT_TR_NOOP("Quit.")), 0, 1, 1) == 1)
			return true;
	}
	new KaduMessageWindow;
	return false;
}

void disableLock()
{
	CloseHandle(mutex);
}
