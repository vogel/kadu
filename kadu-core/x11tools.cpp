/****************************************************************************
*   Copyright (C) 2008  Piotr Dabrowski                                     *
*                                                                           *
*   This program is free software: you can redistribute it and/or modify    *
*   it under the terms of the GNU General Public License as published by    *
*   the Free Software Foundation, either version 2 of the License, or       *
*   (at your option) any later version.                                     *
*                                                                           *
*   This program is distributed in the hope that it will be useful,         *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
*   GNU General Public License for more details.                            *
*                                                                           *
*   You should have received a copy of the GNU General Public License       *
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
*                                                                           *
****************************************************************************/


#include <unistd.h>
#include <stdint.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include "x11tools.h"


typedef struct
{
	uint32_t flags;
	uint32_t functions;
	uint32_t decorations;
	 int32_t input_mode;
	uint32_t status;
} MotifWMHints;


#define MWM_HINTS_DECORATIONS (1L << 1)


bool X11_getCardinal32Property( Display *display, Window window, char *propertyName, long *value )
{
	Atom nameAtom, typeAtom, actual_type_return;
	int actual_format_return, result;
	unsigned long nitems_return, bytes_after_return;
	long *result_array = NULL;
	nameAtom = XInternAtom( display, propertyName, False );
	typeAtom = XInternAtom( display, "CARDINAL"  , False );
	if( nameAtom == None || typeAtom == None )
	{
		return false;
	}
	result = XGetWindowProperty(
			display, window, nameAtom, 0, 1, False, typeAtom,
			&actual_type_return, &actual_format_return, &nitems_return, &bytes_after_return, (unsigned char **)&result_array
		);
	if( result != Success )
	{
		return false;
	}
	if( actual_type_return == None || actual_format_return == 0 )
	{
		return false;
	}
	if( actual_type_return != typeAtom )
	{
		return false;
	}
	*value = result_array[0];
	XFree( result_array );
	return true;
}


long X11_getDesktopsCount( Display *display )
{
	Window root = DefaultRootWindow( display );
	long desktop;
	if( ! X11_getCardinal32Property( display, root, (char *)"_NET_NUMBER_OF_DESKTOPS", &desktop ) )
	{
		return -1;
	}
	return desktop;
}


long X11_getCurrentDesktop( Display *display )
{
	Window root = DefaultRootWindow( display );
	long desktop;
	if( ! X11_getCardinal32Property( display, root, (char *)"_NET_CURRENT_DESKTOP", &desktop ) )
	{
		return -1;
	}
	return desktop;
}


void X11_setCurrentDesktop( Display *display, long desktop )
{
	// generate MouseLeave event
	int rootx, rooty, windowx, windowy;
	Window window = X11_getWindowUnderCursor( display, &rootx, &rooty, &windowx, &windowy );
	if( window != None )
	{
		XEvent xev;
		xev.type                  = LeaveNotify;
		xev.xcrossing.type        = LeaveNotify;
		xev.xcrossing.serial      = 0;
		xev.xcrossing.send_event  = False;
		xev.xcrossing.display     = display;
		xev.xcrossing.window      = window;
		xev.xcrossing.root        = DefaultRootWindow( display );
		xev.xcrossing.subwindow   = None;
		xev.xcrossing.time        = CurrentTime;
		xev.xcrossing.x           = windowx;
		xev.xcrossing.y           = windowy;
		xev.xcrossing.x_root      = rootx;
		xev.xcrossing.y_root      = rooty;
		xev.xcrossing.mode        = NotifyNormal;
		xev.xcrossing.detail      = NotifyNonlinear;
		xev.xcrossing.same_screen = True;
		xev.xcrossing.focus       = True;
		xev.xcrossing.state       = 0;
		XSendEvent( display, window, True, LeaveWindowMask, &xev );
	}
	// change desktop
	XEvent xev;
	xev.type                 = ClientMessage;
	xev.xclient.type         = ClientMessage;
	xev.xclient.serial       = 0;
	xev.xclient.send_event   = True;
	xev.xclient.display      = display;
	xev.xclient.window       = DefaultRootWindow( display );
	xev.xclient.message_type = XInternAtom( display, "_NET_CURRENT_DESKTOP", False );
	xev.xclient.format       = 32;
	xev.xclient.data.l[0]    = desktop;
	xev.xclient.data.l[1]    = CurrentTime;
	xev.xclient.data.l[2]    = 0;
	xev.xclient.data.l[3]    = 0;
	xev.xclient.data.l[4]    = 0;
	XSendEvent( display, DefaultRootWindow( display ), False, SubstructureRedirectMask | SubstructureNotifyMask, &xev );
}


long X11_getDesktopOfWindow( Display *display, Window window )
{
	long desktop;
	if( ! X11_getCardinal32Property( display, window, (char *)"_NET_WM_DESKTOP", &desktop ) )
	{
		return -1;
	}
	return desktop;
}


void X11_moveWindowToDesktop( Display *display, Window window, long desktop )
{
	XWindowAttributes wattr;
	XGetWindowAttributes( display, window, &wattr );
	XEvent xev;
	xev.type                 = ClientMessage;
	xev.xclient.type         = ClientMessage;
	xev.xclient.display      = display;
	xev.xclient.window       = window;
	xev.xclient.message_type = XInternAtom( display, "_NET_WM_DESKTOP", False);
	xev.xclient.format       = 32;
	xev.xclient.data.l[0]    = desktop;
	xev.xclient.data.l[1]    = 2;  /* indicate we are messaging from a pager */
	XSendEvent( display, DefaultRootWindow( display ), False, SubstructureNotifyMask | SubstructureRedirectMask, &xev );
}


bool X11_isWindowVisibleOnDesktop( Display *display, Window window, long desktop )
{
	long desktopofwindow = X11_getDesktopOfWindow( display, window );
	if( desktopofwindow == X11_ALLDESKTOPS )
		return true;
	return ( desktopofwindow == desktop );
}


Window X11_getActiveWindow( Display *display )
{
	Window window;
	int revertto;
	XGetInputFocus( display, &window, &revertto );
	return window;
}


void X11_setActiveWindow( Display *display, Window window )
{
	int time = 0;
	while( time < X11_SETACTIVEWINDOW_TIMEOUT )
	{
		if( X11_isWindowVisibleOnDesktop( display, window, X11_getCurrentDesktop( display ) ) )
			break;
		usleep( X11_SETACTIVEWINDOW_CHECKTIME );
		time += X11_SETACTIVEWINDOW_CHECKTIME;
	}
	if( ! X11_isWindowVisibleOnDesktop( display, window, X11_getCurrentDesktop( display ) ) )
		return;
	XSetInputFocus( display, window, RevertToNone, CurrentTime );
	XRaiseWindow( display, window );
}


Window X11_getWindowUnderCursor( Display *display, int *rootx, int *rooty, int *windowx, int *windowy )
{
	Window root = DefaultRootWindow( display );
	Window window = None;
	int _rootx, _rooty, _windowx, _windowy;
	unsigned int mask;
	XQueryPointer( display, root, &root, &window,
	               ( rootx ? rootx : &_rootx ),
	               ( rooty ? rooty : &_rooty ),
	               ( windowx ? windowx : &_windowx ),
	               ( windowy ? windowy : &_windowy ),
	               &mask
	             );
	return window;
}


Window X11_getInnerMostWindowUnderCursor( Display *display, int *rootx, int *rooty, int *windowx, int *windowy )
{
	Window root = DefaultRootWindow( display );
	Window child = root;
	Window window;
	int _rootx, _rooty, _windowx, _windowy;
	unsigned int mask;
	do
	{
		window = child;
		XQueryPointer( display, window, &root, &child,
		               ( rootx ? rootx : &_rootx ),
		               ( rooty ? rooty : &_rooty ),
		               ( windowx ? windowx : &_windowx ),
		               ( windowy ? windowy : &_windowy ),
		               &mask
		             );
	} while ( ( child != None ) && ( child != window ) );
	return window;
}


std::string X11_getWindowClass( Display *display, Window window )
{
	XClassHint classhint;
	if( XGetClassHint( display, window, &classhint ) == 0 ) // 0 means error
		return "";
	std::string classstring = "";
	classstring += classhint.res_name;
	classstring += " ";
	classstring += classhint.res_class;
	XFree( classhint.res_name );
	XFree( classhint.res_class );
	return classstring;
}


std::string X11_getWindowRole( Display *display, Window window )
{
	XTextProperty textproperty;
	std::string s = "";
	Atom _XA_WM_WINDOW_ROLE = XInternAtom( display, "WM_WINDOW_ROLE", False );
	if( XGetTextProperty( display, window, &textproperty, _XA_WM_WINDOW_ROLE ) )
	{
		if( textproperty.encoding == XA_STRING && textproperty.format == 8 && textproperty.nitems > 0 )
			s += (char *)textproperty.value;
	}
	XFree( textproperty.value );
	return s;
}


void X11_windowSendXEvent( Display *display, Window window, const char *type, const char *message, bool set )
{
	Atom atomtype    = XInternAtom( display, type   , False );
	Atom atommessage = XInternAtom( display, message, False );
	XEvent xev;
	xev.type                 = ClientMessage;
	xev.xclient.type         = ClientMessage;
	xev.xclient.serial       = 0;
	xev.xclient.send_event   = True;
	xev.xclient.window       = window;
	xev.xclient.message_type = atomtype;
	xev.xclient.format       = 32;
	xev.xclient.data.l[0]    = ( set ? 1 : 0 );
	xev.xclient.data.l[1]    = atommessage;
	xev.xclient.data.l[2]    = 0;
	xev.xclient.data.l[3]    = 0;
	xev.xclient.data.l[4]    = 0;
	XSendEvent( display, DefaultRootWindow( display ), False, SubstructureRedirectMask | SubstructureNotifyMask, &xev );
}


void X11_windowChangeProperty( Display *display, Window window, const char *property, const char *value )
{
	Atom atomproperty = XInternAtom( display, property, False );
	Atom atomvalue    = XInternAtom( display, value   , False );
	XChangeProperty( display, window, atomproperty, XA_ATOM, 32, PropModeReplace, (unsigned char *)&atomvalue, 1 );
}


void X11_windowSetDecoration( Display *display, Window window, bool set )
{
	MotifWMHints hints;
	Atom atom = XInternAtom( display, "_MOTIF_WM_HINTS", False );
	if( atom != None)
	{
		hints.flags = MWM_HINTS_DECORATIONS;
		hints.decorations = ( set ? 1 : 0 );
		XChangeProperty( display, window, atom, atom, 32, PropModeReplace, (unsigned char *)&hints, 5 );
	}
}
