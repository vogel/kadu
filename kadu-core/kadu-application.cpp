/*
 * Copyright 2007, 2008, 2009 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2002, 2003, 2004, 2005, 2007 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2002, 2003 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004, 2007, 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008, 2009 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2008, 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2004, 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2002, 2003 Dariusz Jagodzik (mast3r@kadu.net)
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2011, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "kadu-application.h"

#if defined(Q_WS_X11)
#include <X11/Xatom.h>
#include <X11/extensions/Xfixes.h>
#undef KeyPress
#undef Status

#include <QtGui/QX11Info>

#include "os/generic/compositing-aware-object.h"
#endif // Q_WS_X11

KaduApplication::KaduApplication(int &argc, char *argv[]) :
		QApplication(argc, argv)
#if defined(Q_WS_X11)
		, net_wm_state{}
#endif // Q_WS_X11
#if QT_VERSION < 0x050000
		, SavingSession(false)
#endif
{
	setApplicationName("Kadu");
	setQuitOnLastWindowClosed(false);

#ifdef Q_OS_WIN32
	// Fix for #2491
	setStyleSheet("QToolBar{border:0px}");
#endif

#if defined(Q_WS_X11)
	xfixes_event_base = -1;
	int dummy;
	if (XFixesQueryExtension(QX11Info::display(), &xfixes_event_base, &dummy))
	{
		net_wm_state = XInternAtom(QX11Info::display(), "_NET_WM_CM_S0", False);
		XFixesSelectSelectionInput(QX11Info::display(), QX11Info::appRootWindow(0) , net_wm_state,
		XFixesSetSelectionOwnerNotifyMask |
		XFixesSelectionWindowDestroyNotifyMask |
		XFixesSelectionClientCloseNotifyMask);
	}
	if (QX11Info::isCompositingManagerRunning())
		CompositingAwareObject::compositingStateChanged();
#endif // Q_WS_X11
}

#if QT_VERSION < 0x050000
void KaduApplication::commitData(QSessionManager &manager)
{
	SavingSession = true;

	QApplication::commitData(manager);

	SavingSession = false;
}

#if defined(Q_WS_X11)
bool KaduApplication::x11EventFilter(XEvent *event)
{
	if (xfixes_event_base != -1 && event->type == xfixes_event_base + XFixesSelectionNotify)
	{
		XFixesSelectionNotifyEvent *ev = (XFixesSelectionNotifyEvent *)event;
		if (ev->selection == net_wm_state)
			CompositingAwareObject::compositingStateChanged();
	}
	return false;
}
#endif // Q_WS_X11

bool KaduApplication::isSavingSession() const
{
	return SavingSession;
}
#endif

#include "moc_kadu-application.cpp"
