/*
 * %kadu copyright begin%
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

#if defined(Q_WS_X11) && !defined(Q_WS_MAEMO_5)
#include <X11/Xatom.h>
#include <X11/extensions/Xfixes.h>
#undef Bool
#undef Status

#include <QtGui/QX11Info>

#include "os/generic/compositing-aware-object.h"
#endif // Q_WS_X11 && !Q_WS_MAEMO_5

#ifdef Q_OS_MAC
#include "core/core.h"
#include "gui/windows/kadu-window.h"
#include "gui/widgets/chat-widget-manager.h"
#include "chat/message/pending-messages-manager.h"

static OSStatus appleEventProcessor(const AppleEvent *ae, AppleEvent *event, long handlerRefCon)
{
	Q_UNUSED(event)
	Q_UNUSED(handlerRefCon)

	OSType aeID = typeWildCard;
	OSType aeClass = typeWildCard;

	AEGetAttributePtr(ae, keyEventClassAttr, typeType, 0, &aeClass, sizeof(aeClass), 0);
	AEGetAttributePtr(ae, keyEventIDAttr, typeType, 0, &aeID, sizeof(aeID), 0);

	if (aeClass == kCoreEventClass)
	{
		if (aeID == kAEReopenApplication)
		{
			if (PendingMessagesManager::instance()->hasPendingMessages())
				ChatWidgetManager::instance()->openPendingMsgs(true);
			else
				Core::instance()->kaduWindow()->show();
		}
		return noErr;
	}

	return eventNotHandledErr;
}
#endif // Q_OS_MAC

KaduApplication::KaduApplication(int &argc, char *argv[]) :
		QApplication(argc, argv)
{
	setApplicationName("Kadu");

#ifdef Q_OS_MAC
	/* Install Reopen Application Event (Dock Clicked) */
	m_appleEventProcessorUPP = AEEventHandlerUPP(appleEventProcessor);
	AEInstallEventHandler(kCoreEventClass, kAEReopenApplication,
		m_appleEventProcessorUPP, (long) this, true);
#endif // Q_OS_MAC

#if defined(Q_WS_X11) && !defined(Q_WS_MAEMO_5)
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
#endif // Q_WS_X11 && !Q_WS_MAEMO_5
}

// TODO: this is a hack, see KaduWindow::closeEvent()
void KaduApplication::commitData(QSessionManager &manager)
{
	Q_UNUSED(manager)

	qApp->quit();
}

#if defined(Q_WS_X11) && !defined(Q_WS_MAEMO_5)
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
#endif // Q_WS_X11 && !Q_WS_MAEMO_5
