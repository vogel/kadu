/*
 * Copyright (C) 2009 Michal Podsiadlik <michal@kadu.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the Clarified Artistic License (see LICENSE.txt
 * for details).
 */

#include <QtCore/QDir>
#include <QtCore/QStringList>
#include <QtCore/QTextStream>

#include "plugins/mediaplayer/mediaplayer.h"
#include "debug.h"
#include "winamp.h"

#include "wa_ipc.h"

// Winamp control commands
#define WINAMP_CMD_START 40045
#define WINAMP_CMD_PLAY_OR_PAUSE 40046
#define WINAMP_CMD_NEXT_TRACK 40048
#define WINAMP_CMD_PREVIOUS_TRACK  40044
#define WINAMP_CMD_STOP 40047
#define WINAMP_CMD_RAISE_VOLUME 40058
#define WINAMP_CMD_LOWER_VOLUME 40059

WinampMediaPlayer::WinampMediaPlayer()
{
	kdebugf();
}

WinampMediaPlayer::~WinampMediaPlayer()
{
	kdebugf();
}

HWND WinampMediaPlayer::findWinamp()
{
	return FindWindow("Winamp v1.x", NULL);

}

QString WinampMediaPlayer::readWinampMemory(quint32 command, quint32 arg, bool unicode)
{
	HWND hWinamp=findWinamp();
	if(hWinamp){
		void* pResult=(void*)SendMessage(hWinamp, WM_WA_IPC, arg, command);
		if((int)pResult == 1){
			kdebugm(KDEBUG_WARNING, "command %d unsupported by player\n", command);
			return QString();
		}
		DWORD processId;
		GetWindowThreadProcessId(hWinamp, &processId);

		HANDLE hWinampProcess=OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ, 0, processId);
		if(!SUCCEEDED(hWinampProcess)){
			kdebugm(KDEBUG_WARNING, "unable to open winamp process\n");
			return QString();
		}

		char lpBuffer[512];
		if(!SUCCEEDED(ReadProcessMemory(hWinampProcess, pResult, lpBuffer, 512, NULL))){
			kdebugm(KDEBUG_WARNING, "unable to read winamp memory\n");
			CloseHandle(hWinampProcess);
			return QString();
		}

		if(unicode){
			return QString::fromUtf16((const ushort*)lpBuffer);
		}
		else{
			kdebug("ret = %s\n", lpBuffer);
			return QString::fromLocal8Bit(lpBuffer);
		}
	}
	return QString();
}

QString WinampMediaPlayer::getFileTagW(int position, QString tag)
{
	HWND hWinamp=findWinamp();
	if(hWinamp){
		DWORD processId;
		GetWindowThreadProcessId(hWinamp, &processId);

		HANDLE hWinampProcess=OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, 0, processId);
		if(!SUCCEEDED(hWinampProcess)){
			kdebugm(KDEBUG_WARNING, "unable to open winamp process\n");
			return QString();
		}

		extendedFileInfoStructW* pFileInfo = (extendedFileInfoStructW*)VirtualAllocEx(hWinampProcess, NULL, sizeof(extendedFileInfoStructW), MEM_COMMIT, PAGE_READWRITE);
		wchar_t* pForeightTag = (wchar_t*)VirtualAllocEx(hWinampProcess, NULL, 64, MEM_COMMIT, PAGE_READWRITE);
		wchar_t* pForeightRet=(wchar_t*)VirtualAllocEx(hWinampProcess, NULL, 256, MEM_COMMIT, PAGE_READWRITE);

		if(!pFileInfo || ! pForeightTag || !pForeightRet){
			kdebugm(KDEBUG_WARNING, "unable to allocate foreight memory\n");
			CloseHandle(hWinampProcess);
			return QString();
		}

		extendedFileInfoStructW fis;
		fis.filename=(wchar_t*)SendMessage(hWinamp, WM_WA_IPC, position, IPC_GETPLAYLISTFILEW);
		fis.metadata=pForeightTag;
		fis.ret=pForeightRet;
		fis.retlen=256;

		WriteProcessMemory(hWinampProcess, pFileInfo, &fis, sizeof(fis), NULL);

		const wchar_t* lpTag=(const wchar_t*)tag.utf16();

		WriteProcessMemory(hWinampProcess, pForeightTag, lpTag, sizeof(wchar_t) * (wcslen(lpTag) + 1), NULL);

		SendMessage(hWinamp, WM_WA_IPC, (WPARAM)pFileInfo, IPC_GET_EXTENDED_FILE_INFOW);

		wchar_t lpReturn[128];

		ReadProcessMemory(hWinampProcess, pForeightRet, lpReturn, 256, NULL);

		VirtualFreeEx(hWinampProcess, pForeightRet, 0, MEM_RELEASE);
		VirtualFreeEx(hWinampProcess, pForeightTag, 0, MEM_RELEASE);
		VirtualFreeEx(hWinampProcess, pFileInfo, 0, MEM_RELEASE);

		CloseHandle(hWinampProcess);

		return QString::fromUtf16((const ushort*)lpReturn);
	}
	return QString();
}

QString WinampMediaPlayer::getFileTagA(int position, QString tag)
{
	HWND hWinamp=findWinamp();
	if(hWinamp){
		DWORD processId;
		GetWindowThreadProcessId(hWinamp, &processId);

		HANDLE hWinampProcess=OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, 0, processId);
		if(!SUCCEEDED(hWinampProcess)){
			kdebugm(KDEBUG_WARNING, "unable to open winamp process\n");
			return QString();
		}

		extendedFileInfoStruct* pFileInfo = (extendedFileInfoStruct*)VirtualAllocEx(hWinampProcess, NULL, sizeof(extendedFileInfoStruct), MEM_COMMIT, PAGE_READWRITE);
		char* pForeightTag = (char*)VirtualAllocEx(hWinampProcess, NULL, 64, MEM_COMMIT, PAGE_READWRITE);
		char* pForeightRet=(char*)VirtualAllocEx(hWinampProcess, NULL, 128, MEM_COMMIT, PAGE_READWRITE);

		if(!pFileInfo || ! pForeightTag || !pForeightRet){
			kdebugm(KDEBUG_WARNING, "unable to allocate foreight memory\n");
			CloseHandle(hWinampProcess);
			return QString();
		}

		extendedFileInfoStruct fis;
		fis.filename=(char*)SendMessage(hWinamp, WM_WA_IPC, position, IPC_GETPLAYLISTFILE);
		fis.metadata=pForeightTag;
		fis.ret=pForeightRet;
		fis.retlen=128;

		WriteProcessMemory(hWinampProcess, pFileInfo, &fis, sizeof(fis), NULL);

		const char* lpTag=qPrintable(tag);

		WriteProcessMemory(hWinampProcess, pForeightTag, lpTag, strlen(lpTag) + 1, NULL);

		SendMessage(hWinamp, WM_WA_IPC, (WPARAM)pFileInfo, IPC_GET_EXTENDED_FILE_INFO);

		char lpReturn[128];

		ReadProcessMemory(hWinampProcess, pForeightRet, lpReturn, 128, NULL);

		VirtualFreeEx(hWinampProcess, pForeightRet, 0, MEM_RELEASE);
		VirtualFreeEx(hWinampProcess, pForeightTag, 0, MEM_RELEASE);
		VirtualFreeEx(hWinampProcess, pFileInfo, 0, MEM_RELEASE);

		CloseHandle(hWinampProcess);

		return QString::fromLocal8Bit((const char*)lpReturn);
	}
	return QString();
}

// PlayerInfo
QString WinampMediaPlayer::getPlayerName()
{
	kdebugf();

	return "Winamp";
}

QString WinampMediaPlayer::getPlayerVersion()
{
	kdebugf();
	HWND hWinamp=findWinamp();
	if(hWinamp){
		int version=SendMessage(hWinamp, WM_WA_IPC, 0, IPC_GETVERSION);
		return QString("%1.%2").arg(WINAMP_VERSION_MAJOR(version)).arg(WINAMP_VERSION_MINOR(version));
	}
	return QString();
}

QStringList WinampMediaPlayer::getPlayListTitles()
{
	kdebugf();
	QStringList ret;
	HWND hWinamp=findWinamp();
	if(hWinamp){
		int items=getPlayListLength();
		for(int i=0;i<items;i++){
			QString title=readWinampMemory(IPC_GETPLAYLISTTITLEW, i, true);
			if(title.isEmpty())
				title=readWinampMemory(IPC_GETPLAYLISTTITLE, i);

			if(!title.isEmpty())
				ret.append(title);
		}
	}
	return ret;
}

QStringList WinampMediaPlayer::getPlayListFiles()
{
	kdebugf();
	QStringList ret;
	HWND hWinamp=findWinamp();
	if(hWinamp){
		int items=getPlayListLength();
		for(int i=0;i<items;i++)
			ret.append(getFileImpl(i));
	}
	return ret;
}

uint WinampMediaPlayer::getPlayListLength()
{
	kdebugf();
	HWND hWinamp=findWinamp();
	if(hWinamp)
		return SendMessage(hWinamp, WM_WA_IPC, 0, IPC_GETLISTLENGTH);

	return 0;
}

QString WinampMediaPlayer::getTitle()
{
	kdebugf();
	HWND hWinamp=findWinamp();
	if(hWinamp){
		int position=SendMessage(hWinamp, WM_WA_IPC, 0, IPC_GETLISTPOS);

		QString title = getFileTagW(position, "title");

		if(title.isEmpty()){
			title = getFileTagA(position, "title");
		}

		return title;
	}
	return QString();
}

QString WinampMediaPlayer::getAlbum()
{
	kdebugf();
	HWND hWinamp=findWinamp();
	if(hWinamp){
		int position=SendMessage(hWinamp, WM_WA_IPC, 0, IPC_GETLISTPOS);

		QString album = getFileTagW(position, "album");

		if(album.isEmpty()){
			album = getFileTagA(position, "album");
		}

		return album;
	}
	return QString();
}

QString WinampMediaPlayer::getArtist()
{
	kdebugf();
	HWND hWinamp=findWinamp();
	if(hWinamp){
		int position=SendMessage(hWinamp, WM_WA_IPC, 0, IPC_GETLISTPOS);

		QString artist = getFileTagW(position, "artist");

		if(artist.isEmpty()){
			artist = getFileTagA(position, "artist");
		}

		return artist;
	}
	return "";
}

QString WinampMediaPlayer::getFileImpl(int position)
{
	kdebugf();
	HWND hWinamp=findWinamp();
	if(hWinamp){
		if (position < 0)
			position=SendMessage(hWinamp, WM_WA_IPC, 0, IPC_GETLISTPOS);

		QString file=readWinampMemory(IPC_GETPLAYLISTFILEW, position, true);
		if(file.isEmpty()){
			kdebugm(KDEBUG_INFO, "not unicode capable..\n");
			file=readWinampMemory(IPC_GETPLAYLISTFILE, position);
		}

		return file;
	}

	return QString();
}

QString WinampMediaPlayer::getFile()
{
	return getFileImpl(-1);
}

int WinampMediaPlayer::getLength()
{
	kdebugf();
	HWND hWinamp=findWinamp();

	if(hWinamp){
		return SendMessage(hWinamp, WM_WA_IPC, 2, IPC_GETOUTPUTTIME);
	}

	return 0;
}

int WinampMediaPlayer::getCurrentPos()
{
	kdebugf();
	HWND hWinamp=findWinamp();
	if(hWinamp)
		return SendMessage(hWinamp, WM_WA_IPC, 0, IPC_GETOUTPUTTIME);

	return 0;
}

bool WinampMediaPlayer::isPlaying()
{
	kdebugf();
	HWND hWinamp=findWinamp();
	if(hWinamp){
		return (SendMessage(hWinamp, WM_WA_IPC, 0, IPC_ISPLAYING) == 1);
	}

	return 0;
}

bool WinampMediaPlayer::isActive()
{
	kdebugf();

	return findWinamp();
}

// PlayerCommands
void WinampMediaPlayer::nextTrack()
{
	kdebugf();
	HWND hWinamp=findWinamp();
	if(hWinamp)
		SendMessage(hWinamp, WM_COMMAND, WINAMP_CMD_NEXT_TRACK, 1);
}

void WinampMediaPlayer::prevTrack()
{
	kdebugf();
	HWND hWinamp=findWinamp();
	if(hWinamp)
		SendMessage(hWinamp, WM_COMMAND, WINAMP_CMD_PREVIOUS_TRACK, 1);
}

void WinampMediaPlayer::play()
{
	kdebugf();
	HWND hWinamp=findWinamp();
	if(hWinamp)
		SendMessage(hWinamp, WM_COMMAND, WINAMP_CMD_START, 1);
}

void WinampMediaPlayer::stop()
{
	kdebugf();
	HWND hWinamp=findWinamp();
	if(hWinamp)
		SendMessage(hWinamp, WM_COMMAND, WINAMP_CMD_STOP, 1);
}

void WinampMediaPlayer::pause()
{
	kdebugf();
	HWND hWinamp=findWinamp();
	if(hWinamp)
		SendMessage(hWinamp, WM_COMMAND, WINAMP_CMD_PLAY_OR_PAUSE, 1);
}

void WinampMediaPlayer::setVolume(int vol)
{
	kdebugf();
	HWND hWinamp=findWinamp();
	if(hWinamp)
		SendMessage(hWinamp, WM_WA_IPC, (vol*255)/100, IPC_SETVOLUME);
}

void WinampMediaPlayer::incrVolume()
{
	kdebugf();
	HWND hWinamp=findWinamp();
	if(hWinamp)
		SendMessage(hWinamp, WM_COMMAND, WINAMP_CMD_RAISE_VOLUME, 1);
}

void WinampMediaPlayer::decrVolume()
{
	kdebugf();
	HWND hWinamp=findWinamp();
	if(hWinamp)
		SendMessage(hWinamp, WM_COMMAND, WINAMP_CMD_LOWER_VOLUME, 1);
}
