/*
  Copyright (c) 2000 Troll Tech AS
  Copyright (c) 2003 Lubos Lunak <l.lunak@kde.org>

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.
*/

#include "activate.h"

static Atom net_active_window = 0;

void create_netwm_atoms(Display *d)
{
	static const char * const names[1] =
	{
	    "_NET_ACTIVE_WINDOW",
	};

	Atom atoms[1], *atomsp[1] =
	{
	    &net_active_window,
	};

	int i = 1;
	while (i--)
		atoms[i] = 0;

	XInternAtoms(d, (char **) names, 1, False, atoms);

	i = 1;
	while (i--)
		*atomsp[i] = atoms[i];
}

const unsigned long netwm_sendevent_mask = (SubstructureRedirectMask | SubstructureNotifyMask);

void activateWindow(WId id)
{
	XEvent e;

	e.xclient.type = ClientMessage;
	e.xclient.message_type = net_active_window;
	e.xclient.display = qt_xdisplay(); // QX11Info::display();
	e.xclient.window = id;
	e.xclient.format = 32;
	e.xclient.data.l[0] = 2l; // NET::FromApplication;
	e.xclient.data.l[1] = 0l;
	e.xclient.data.l[2] = id;
	e.xclient.data.l[3] = 0l;
	e.xclient.data.l[4] = 0l;

	XSendEvent(qt_xdisplay(), qt_xrootwin(), False, netwm_sendevent_mask, &e);
}
