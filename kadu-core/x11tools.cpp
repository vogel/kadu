/****************************************************************************
*   Copyright (C) 2008-2009  Piotr Dabrowski                                *
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
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include "x11tools.h"




bool X11_getCardinalProperty( Display *display, Window window, const char *propertyName, long *value, int offset )
{
	Atom nameAtom, actual_type_return;
	int actual_format_return, result;
	unsigned long nitems_return, bytes_after_return;
	unsigned char *value_return = NULL;
	nameAtom = XInternAtom( display, propertyName, False );
	if( nameAtom == None )
	{
		return false;
	}
	result = XGetWindowProperty(
			display, window, nameAtom, offset, 1, False, XA_CARDINAL,
			&actual_type_return, &actual_format_return, &nitems_return, &bytes_after_return, &value_return
		);
	if( result == Success )
	{
		if( actual_type_return == XA_CARDINAL && nitems_return > 0 )
		{
			*value = ((long*)value_return)[0];
			XFree( value_return );
			return true;
		}
		XFree( value_return );
	}
	return false;
}


bool X11_getAtomProperty( Display *display, Window window, const char *propertyName, Atom *value )
{
	Atom nameAtom, actual_type_return;
	int actual_format_return, result;
	unsigned long nitems_return, bytes_after_return;
	unsigned char *value_return = NULL;
	nameAtom = XInternAtom( display, propertyName, False );
	if( nameAtom == None )
	{
		return false;
	}
	result = XGetWindowProperty(
			display, window, nameAtom, 0, 1, False, XA_ATOM,
			&actual_type_return, &actual_format_return, &nitems_return, &bytes_after_return, &value_return
		);
	if( result == Success )
	{
		if( actual_type_return == XA_ATOM && nitems_return > 0 )
		{
			*value = ((Atom*)value_return)[0];
			XFree( value_return );
			return true;
		}
		XFree( value_return );
	}
	return false;
}




std::pair<int,int> X11_getResolution( Display *display )
{
	// we can use _NET_WORKAREA as well
	return std::make_pair( DisplayWidth( display, 0 ), DisplayHeight( display, 0 ) );
}


std::pair<int,int> X11_getDesktopSize( Display *display )
{
	long width;
	if( ! X11_getCardinalProperty( display, DefaultRootWindow( display ), "_NET_DESKTOP_GEOMETRY", &width, 0 ) )
	{
		return std::make_pair( 0, 0 );
	}
	long height;
	if( ! X11_getCardinalProperty( display, DefaultRootWindow( display ), "_NET_DESKTOP_GEOMETRY", &height, 1 ) )
	{
		return std::make_pair( 0, 0 );
	}
	return std::make_pair( width, height );
}




std::pair<int,int> X11_getMousePos( Display *display )
{
	int x = 0;
	int y = 0;
	Window root;
	Window child;
	int winx, winy;
	unsigned int mask;
	XQueryPointer( display, DefaultRootWindow( display ), &root, &child, &x, &y, &winx, &winy, &mask );
	return std::make_pair( x, y );
}




bool X11_isFreeDesktopCompatible( Display *display )
{
	if( X11_getDesktopsCount( display, true ) != 1 )
	{
		// _NET multiple desktops, FreeDesktop compatible
		return true;
	}
	std::pair<int,int> resolution  = X11_getResolution(  display );
	std::pair<int,int> desktopsize = X11_getDesktopSize( display );
	if( resolution == desktopsize )
	{
		// one desktop only, so we don't have to care
		return true;
	}
	if( ( desktopsize.first % resolution.first != 0 ) || ( desktopsize.second % resolution.second != 0 ) )
	{
		// virtual resolution
		return true;
	}
	// not FreeDesktop compatible :(
	return false;
}




long X11_getDesktopsCount( Display *display, bool forceFreeDesktop )
{
	if( ( ! forceFreeDesktop ) && ( ! X11_isFreeDesktopCompatible( display ) ) )
	{
		std::pair<int,int> resolution  = X11_getResolution(  display );
		std::pair<int,int> desktopsize = X11_getDesktopSize( display );
		return ( desktopsize.second / resolution.second ) * ( desktopsize.first / resolution.first ) ;
	}
	else
	{
		long value;
		if( ! X11_getCardinalProperty( display, DefaultRootWindow( display ), "_NET_NUMBER_OF_DESKTOPS", &value ) )
		{
			return 0;
		}
		return value;
	}
}


long X11_getCurrentDesktop( Display *display, bool forceFreeDesktop )
{
	if( ( ! forceFreeDesktop ) && ( ! X11_isFreeDesktopCompatible( display ) ) )
	{
		long dx, dy;
		X11_getCardinalProperty( display, DefaultRootWindow( display ), "_NET_DESKTOP_VIEWPORT", &dx, 0 );
		X11_getCardinalProperty( display, DefaultRootWindow( display ), "_NET_DESKTOP_VIEWPORT", &dy, 1 );
		std::pair<int,int> desktopsize = X11_getDesktopSize( display );
		std::pair<int,int> resolution = X11_getResolution( display );
		long desktop = ( dy / resolution.second ) * ( desktopsize.first / resolution.first ) + ( dx / resolution.first );
		return desktop;
	}
	else
	{
		long desktop;
		if( ! X11_getCardinalProperty( display, DefaultRootWindow( display ), "_NET_CURRENT_DESKTOP", &desktop ) )
		{
			return X11_BADDESKTOP;
		}
		return desktop;
	}
}


void X11_setCurrentDesktop( Display *display, long desktop, bool forceFreeDesktop )
{
	if( desktop != X11_getCurrentDesktop( display ) )
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
			XFlush( display );
		}
		// change desktop
		if( ( ! forceFreeDesktop ) && ( ! X11_isFreeDesktopCompatible( display ) ) )
		{
			std::pair<int,int> desktopsize = X11_getDesktopSize( display );
			std::pair<int,int> resolution = X11_getResolution( display );
			long dx = ( desktop % ( desktopsize.first / resolution.first ) ) * resolution.first;
			long dy = ( desktop / ( desktopsize.first / resolution.first ) ) * resolution.second;
			XEvent xev;
			xev.type                 = ClientMessage;
			xev.xclient.type         = ClientMessage;
			xev.xclient.serial       = 0;
			xev.xclient.send_event   = True;
			xev.xclient.display      = display;
			xev.xclient.window       = DefaultRootWindow( display );
			xev.xclient.message_type = XInternAtom( display, "_NET_DESKTOP_VIEWPORT", False );
			xev.xclient.format       = 32;
			xev.xclient.data.l[0]    = dx;
			xev.xclient.data.l[1]    = dy;
			xev.xclient.data.l[2]    = 0;
			xev.xclient.data.l[3]    = 0;
			xev.xclient.data.l[4]    = 0;
			XSendEvent( display, DefaultRootWindow( display ), False, SubstructureRedirectMask | SubstructureNotifyMask, &xev );
			XFlush( display );
		}
		else
		{
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
			XFlush( display );
		}
	}
}


long X11_getDesktopOfWindow( Display *display, Window window, bool forceFreeDesktop, bool windowareadecides )
{
	if( ( ! forceFreeDesktop ) && ( ! X11_isFreeDesktopCompatible( display ) ) )
	{
		long currentdesktop = X11_getCurrentDesktop( display );
		std::pair<int,int> pos = X11_getWindowPos( display, window );
		std::pair<int,int> desktopsize = X11_getDesktopSize( display );
		std::pair<int,int> resolution = X11_getResolution( display );
		if( windowareadecides )
		{
			std::pair<int,int> size = X11_getWindowSize( display, window );
			pos.first  += size.first  / 2;
			pos.second += size.second / 2;
			pos.second %= desktopsize.second;
		}
		long desktopofwindow = currentdesktop + ( pos.second / resolution.second ) * ( desktopsize.first / resolution.first ) + ( pos.first / resolution.first );
		if( pos.first < 0 )
		{
			desktopofwindow -= 1;
		}
		if( pos.second < 0 )
		{
			desktopofwindow -= ( desktopsize.first / resolution.first );
		}
		desktopofwindow %= X11_getDesktopsCount( display );
		return desktopofwindow;
	}
	else
	{
		long desktopofwindow;
		if( ! X11_getCardinalProperty( display, window, "_NET_WM_DESKTOP", &desktopofwindow ) )
		{
			return X11_BADDESKTOP;
		}
		return desktopofwindow;
	}
}


void X11_moveWindowToDesktop( Display *display, Window window, long desktop, bool forceFreeDesktop, bool position, int x, int y )
{
	if( ( ! forceFreeDesktop ) && ( ! X11_isFreeDesktopCompatible( display ) ) )
	{
		std::pair<int,int> pos = X11_getWindowPos( display, window );
		std::pair<int,int> desktopsize = X11_getDesktopSize( display );
		std::pair<int,int> resolution = X11_getResolution( display );
		long desktopofwindow = X11_getDesktopOfWindow( display, window );
		long ddx = ( desktop % ( desktopsize.first / resolution.first ) ) - ( desktopofwindow % ( desktopsize.first / resolution.first ) );
		long ddy = ( desktop / ( desktopsize.first / resolution.first ) ) - ( desktopofwindow / ( desktopsize.first / resolution.first ) );
		int newx, newy;
		if( position )
		{
			int oldx = pos.first % resolution.first;
			if( oldx < 0 )
			{
				oldx += resolution.first;
			}
			int oldy = pos.second % resolution.second;
			if( oldy < 0 )
			{
				oldy += resolution.second;
			}
			newx = ( pos.first  - oldx + x ) + ddx * resolution.first;
			newy = ( pos.second - oldy + y ) + ddy * resolution.second;
		}
		else
		{
			newx = pos.first  + ddx * resolution.first;
			newy = pos.second + ddy * resolution.second;
		}
		X11_moveWindow( display, window, newx, newy );
	}
	else
	{
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
		XFlush( display );
		if( position )
		{
			X11_moveWindow( display, window, x, y );
		}
	}
}


bool X11_isWindowVisibleOnDesktop( Display *display, Window window, long desktop, bool forceFreeDesktop )
{
	if( ( ! forceFreeDesktop ) && ( ! X11_isFreeDesktopCompatible( display ) ) )
	{
		long desktopofwindow = X11_getDesktopOfWindow( display, window, forceFreeDesktop );
		return ( desktopofwindow == desktop );
	}
	else
	{
		long desktopofwindow = X11_getDesktopOfWindow( display, window, forceFreeDesktop );
		if( desktopofwindow == (long)X11_ALLDESKTOPS )
			return true;
		return ( desktopofwindow == desktop );
	}
}


bool X11_isWholeWindowOnOneDesktop( Display *display, Window window )
{
	std::pair<int,int> pos = X11_getWindowPos( display, window );
	std::pair<int,int> size = X11_getWindowSize( display, window );
	std::pair<int,int> resolution = X11_getResolution( display );
	if( ( pos.first < 0 ) && ( pos.first + size.first > 0 ) )
		return false;
	if( ( pos.first > 0 ) && ( pos.first + size.first < 0 ) )
		return false;
	if( ( pos.second < 0 ) && ( pos.second + size.second > 0 ) )
		return false;
	if( ( pos.second > 0 ) && ( pos.second + size.second < 0 ) )
		return false;
	if( ( pos.first / resolution.first ) != ( ( pos.first + size.first ) / resolution.first ) )
		return false;
	if( ( pos.second / resolution.second ) != ( ( pos.second + size.second ) / resolution.second ) )
		return false;
	return true;
}


bool X11_isWindowFullyVisible( Display *display, Window window )
{
	Window parent = window;
	Window root;
	Window *children = NULL;
	unsigned int nchildren;
	while( parent != DefaultRootWindow( display ) )
	{
		window = parent;
		XQueryTree( display, window, &root, &parent, &children, &nchildren );
		XFree( children );
	}
	int x, y;
	unsigned int width, height, border, depth;
	XGetGeometry( display, window, &root, &x, &y, &width, &height, &border, &depth );
	int x1 = x;
	int y1 = y;
	int x2 = x + width;
	int y2 = y + height;
	XQueryTree( display, DefaultRootWindow( display ), &root, &parent, &children, &nchildren );
	if( children != NULL )
	{
		unsigned int k = 0;
		while( k < nchildren )
		{
			if( children[k] == window )
			{
				break;
			}
			k++;
		}
		Atom typedock = XInternAtom( display, "_NET_WM_WINDOW_TYPE_DOCK", False );
		Atom typemenu = XInternAtom( display, "_NET_WM_WINDOW_TYPE_MENU", False );
		while( k < nchildren )
		{
			if( children[k] == window )
			{
				k++;
				continue;
			}
			XWindowAttributes attr;
			XGetWindowAttributes( display, children[k], &attr );
			if( attr.map_state != IsViewable )
			{
				k++;
				continue;
			}
			std::pair<int,int> pos2 = X11_getWindowPos( display, children[k] );
			std::pair<int,int> size2 = X11_getWindowSize( display, children[k] );
			int X1 = pos2.first;
			int Y1 = pos2.second;
			int X2 = pos2.first + size2.first;
			int Y2 = pos2.second + size2.second;
			if( ( ( x1 <= X1 && X1 <= x2 ) || ( x1 <= X2 && X2 <= x2 ) || ( x1 >= X1 && x2 <= X2 ) )
			&&  ( ( y1 <= Y1 && Y1 <= y2 ) || ( y1 <= Y2 && Y2 <= y2 ) || ( y1 >= Y1 && y2 <= Y2 ) )
			)
			{
				Atom type;
				bool hastype = false;
				if( X11_getAtomProperty( display, children[k], "_NET_WM_WINDOW_TYPE", &type ) )
				{
					hastype = true;
				}
				if( ! hastype )
				{
					Window window2 = children[k];
					Window *children2 = NULL;
					unsigned int nchildren2;
					while( ( ! hastype ) )
					{
						XQueryTree( display, window2, &root, &parent, &children2, &nchildren2 );
						if( children2 != NULL )
						{
							window2 = children2[0];
							if( X11_getAtomProperty( display, window2, "_NET_WM_WINDOW_TYPE", &type ) )
							{
								hastype = true;
							}
							XFree( children2 );
						}
						else
						{
							break;
						}
					}
				}
				if( ( hastype ) && ( type != typedock ) && ( type != typemenu ) )
				{
					XFree( children );
					return false;
				}
			}
			k++;
		}
		XFree( children );
	}
	return true;
}




std::pair<int,int> X11_getWindowPos( Display *display, Window window )
{
	Window parent = window;
	Window root;
	Window *children;
	unsigned int nchildren;
	while( parent != DefaultRootWindow( display ) )
	{
		window = parent;
		if( XQueryTree( display, window, &root, &parent, &children, &nchildren ) == 0 )
		{
			return std::make_pair( 0, 0 );
		}
		XFree( children );
	}
	if( window == DefaultRootWindow( display ) )
	{
		return std::make_pair( 0, 0 );
	}
	int x, y;
	unsigned int width, height, border, depth;
	if( XGetGeometry( display, window, &root, &x, &y, &width, &height, &border, &depth ) == 0 )
	{
		return std::make_pair( 0, 0 );
	}
	return std::make_pair( x, y );
}


std::pair<int,int> X11_getWindowSize( Display *display, Window window )
{
	Window parent = window;
	Window root;
	Window *children;
	unsigned int nchildren;
	while( parent != DefaultRootWindow( display ) )
	{
		window = parent;
		if( XQueryTree( display, window, &root, &parent, &children, &nchildren ) == 0 )
		{
			return std::make_pair( 0, 0 );
		}
		XFree( children );
	}
	if( window == DefaultRootWindow( display ) )
	{
		return std::make_pair( 0, 0 );
	}
	int x, y;
	unsigned int width, height, border, depth;
	if( XGetGeometry( display, window, &root, &x, &y, &width, &height, &border, &depth ) == 0 )
	{
		return std::make_pair( 0, 0 );
	}
	return std::make_pair( width + 2*border, height + 2*border );
}


std::pair<int,int> X11_getWindowFramelessSize( Display *display, Window window )
{
	Window root;
	int x, y;
	unsigned int width, height, border, depth;
	if( XGetGeometry( display, window, &root, &x, &y, &width, &height, &border, &depth ) == 0 )
	{
		return std::make_pair( 0, 0 );
	}
	return std::make_pair( width + 2*border, height + 2*border );
}


void X11_moveWindow( Display *display, Window window, int x, int y )
{
	XMoveWindow( display, window, x, y );
	XFlush( display );
}


void X11_centerWindow( Display *display, Window window, long desktop, bool forceFreeDesktop )
{
	if( desktop == (long)X11_BADDESKTOP )
	{
		desktop = X11_getCurrentDesktop( display );
	}
	if( ( ! forceFreeDesktop ) && ( ! X11_isFreeDesktopCompatible( display ) ) )
	{
		std::pair<int,int> resolution = X11_getResolution( display );
		std::pair<int,int> size = X11_getWindowSize( display, window );
		int cx = ( resolution.first  - size.first  ) / 2;
		int cy = ( resolution.second - size.second ) / 2;
		X11_moveWindowToDesktop( display, window, desktop, false, true, cx, cy );
	}
	else
	{
		if( X11_getDesktopOfWindow( display, window, true ) != desktop )
		{
			X11_moveWindowToDesktop( display, window, desktop, true );
		}
		std::pair<int,int> resolution = X11_getResolution( display );
		std::pair<int,int> size = X11_getWindowSize( display, window );
		int cx = ( resolution.first  - size.first  ) / 2;
		int cy = ( resolution.second - size.second ) / 2;
		X11_moveWindow( display, window, cx, cy );
	}
}


void X11_resizeWindow( Display *display, Window window, int width, int height )
{
	XResizeWindow( display, window, width, height );
	XFlush( display );
}




Window X11_getActiveWindow( Display *display )
{
	Window window;
	int revertto;
	XGetInputFocus( display, &window, &revertto );
	return window;
}


void X11_setActiveWindow( Display *display, Window window, bool forceFreeDesktop )
{
	forceFreeDesktop = false; // no "unused parameter" compiler warning
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
	XRaiseWindow( display, window );
	XSetInputFocus( display, window, RevertToNone, CurrentTime );
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
	XFlush( display );
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
