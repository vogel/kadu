/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <windows.h>

#include "../sound/sound.h"

#include "debug.h"

#include "player_dshow.h"

#include <dshow.h>

// add strmiids.lib to libs
#pragma comment(lib, "strmiids.lib")

IGraphBuilder *dshow = NULL;
IMediaControl *control = NULL;

/**
 * @ingroup dshow_sound
 * @{
 */
extern "C" KADU_EXPORT int dshow_sound_init(bool firstLoad)
{
	kdebugf();

	dshow_player = new DShowPlayer();

	kdebugf2();
	return 0;
}
extern "C" KADU_EXPORT void dshow_sound_close()
{
	kdebugf();

	delete dshow_player;
	dshow_player = 0;

	kdebugf2();
}

DShowPlayer::DShowPlayer()
{
	kdebugf();

	CoInitialize(NULL);

	CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC,
			IID_IGraphBuilder, (void **)&dshow);

	dshow->QueryInterface(IID_IMediaControl, (void **)&control);

	connect(sound_manager, SIGNAL(playSound(const QString &, bool, double)),
			this, SLOT(playSound(const QString &, bool, double)));

	kdebugf2();
}

DShowPlayer::~DShowPlayer()
{
	kdebugf();

	disconnect(sound_manager, SIGNAL(playSound(const QString &, bool, double)),
			this, SLOT(playSound(const QString &, bool, double)));

	kdebugf2();
}

void DShowPlayer::playSound(const QString &s, bool volCntrl, double vol)
{
	kdebugf();
	control->Stop();
	dshow->RenderFile((LPCWSTR)s.utf16(), 0);
	control->Run();
}

DShowPlayer *dshow_player;

/** @} */
