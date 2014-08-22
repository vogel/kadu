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
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2012, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef KADU_APPLICATION_H
#define KADU_APPLICATION_H

#include <QtGui/QApplication>

#ifdef Q_OS_MAC
#include <Carbon/Carbon.h>
#endif // Q_OS_MAC

#if defined(Q_WS_X11)
#include <X11/Xdefs.h>
#undef KeyPress
#undef Status
#endif // Q_WS_X11

#include "exports.h"

class KADUAPI KaduApplication : public QApplication
{
	Q_OBJECT

#if defined(Q_WS_X11)
	Atom net_wm_state;
	int xfixes_event_base;
#endif // Q_WS_X11
#ifdef Q_OS_MAC
	AEEventHandlerUPP m_appleEventProcessorUPP;
#endif // Q_OS_MAC

#if QT_VERSION < 0x050000
	bool SavingSession;
#endif

public:
	KaduApplication(int &argc, char *argv[]);

#if QT_VERSION < 0x050000
	virtual void commitData(QSessionManager &manager);

#if defined(Q_WS_X11)
	virtual bool x11EventFilter(XEvent *event);
#endif // Q_WS_X11

	bool isSavingSession() const;
#endif

};

#endif // KADU_APPLICATION_H
