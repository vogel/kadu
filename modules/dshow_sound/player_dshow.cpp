/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
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
#include <windows.h>

#include "../sound/sound.h"

#include "debug.h"

#include <dshow.h>

#include "player_dshow.h"

// add strmiids.lib to libs
#pragma comment(lib, "strmiids.lib")

#define WM_MEDIA_NOTIFY WM_APP+1

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

DShowPlayer::DShowPlayer(): dshow(NULL)
{
	kdebugf();

	CoInitialize(NULL);

	connect(sound_manager, SIGNAL(playSound(const QString &, bool, double)),
			this, SLOT(playSound(const QString &, bool, double)));

	kdebugf2();
}

DShowPlayer::~DShowPlayer()
{
	kdebugf();

	disconnect(sound_manager, SIGNAL(playSound(const QString &, bool, double)),
			this, SLOT(playSound(const QString &, bool, double)));

	if(dshow){
		control->Stop();
		event->Release();
		control->Release();
		dshow->Release();
	}

	kdebugf2();
}

void DShowPlayer::playSound(const QString &s, bool volCntrl, double vol)
{
	kdebugf();
	if(!dshow){
		CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC,
				IID_IGraphBuilder, (void **)&dshow);

		dshow->QueryInterface(IID_IMediaControl, (void **)&control);
		dshow->QueryInterface(IID_IMediaEventEx, (void **)&event);

		event->SetNotifyWindow((OAHWND)winId(), WM_MEDIA_NOTIFY, NULL);

		dshow->RenderFile((LPCWSTR)s.utf16(), 0);
		control->Run();
	}
}

bool DShowPlayer::winEvent(MSG * message, long * result)
{
	if(message->message==WM_MEDIA_NOTIFY)
	{
		long code, x1, x2;
		while(SUCCEEDED(event->GetEvent(&code, &x1, &x2, 0)))
		{
			event->FreeEventParams(code, x1, x2);

			if(code==EC_COMPLETE){
				control->Stop();
				event->Release();
				control->Release();
				dshow->Release();
				dshow=NULL;
			}
			return false;
		}

	}
	return QWidget::winEvent(message, result);
}

DShowPlayer *dshow_player;

/** @} */
