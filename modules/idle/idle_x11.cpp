/*
 * idle_x11.cpp - detect desktop idle time
 * Copyright (C) 2003  Justin Karneges
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "idle.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/scrnsaver.h>

static Display *display = 0;
static XScreenSaverInfo *ss_info = 0;

Idle::Idle(QObject *parent) :
		QObject(parent)
{
	if (!ss_info)
	{
		display = XOpenDisplay(0);

		int event_base = 0, error_base = 0;
		if (XScreenSaverQueryExtension(display, &event_base, &error_base))
			ss_info = XScreenSaverAllocInfo();
	}
}

Idle::~Idle()
{
	if (ss_info)
	{
		XFree(ss_info);
		ss_info = 0;
	}

	if (display)
	{
		XCloseDisplay(display);
		display = 0;
	}
}

int Idle::secondsIdle()
{
	if (!ss_info)
		return -1;

	if (!XScreenSaverQueryInfo(display, DefaultRootWindow(display), ss_info))
		return -1;

	return ss_info->idle / 1000;
}
