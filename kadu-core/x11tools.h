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


#ifndef __X11TOOLS_H
	#define __X11TOOLS_H


#include <string>
#include <X11/Xlib.h>


#define  X11_ALLDESKTOPS                0xFFFFFFFF  /*long*/
#define  X11_SETACTIVEWINDOW_TIMEOUT       50*1000  /*usec*/
#define  X11_SETACTIVEWINDOW_CHECKTIME      2*1000  /*usec*/


bool X11_getCardinal32Property( Display *display, Window window, char *propertyName, long *value );

long X11_getDesktopsCount( Display *display );
long X11_getCurrentDesktop( Display *display );
void X11_setCurrentDesktop( Display *display, long desktop );
long X11_getDesktopOfWindow( Display *display, Window window );
void X11_moveWindowToDesktop( Display *display, Window window, long desktop );
bool X11_isWindowVisibleOnDesktop( Display *display, Window window, long desktop );
Window X11_getActiveWindow( Display *display );
void X11_setActiveWindow( Display *display, Window window );
Window X11_getWindowUnderCursor( Display *display, int *rootx = NULL, int *rooty = NULL, int *windowx = NULL, int *windowy = NULL );
Window X11_getInnerMostWindowUnderCursor( Display *display, int *rootx = NULL, int *rooty = NULL, int *windowx = NULL, int *windowy = NULL );
std::string X11_getWindowClass( Display *display, Window window );
std::string X11_getWindowRole( Display *display, Window window );
void X11_windowSendXEvent( Display *display, Window window, const char *type, const char *message, bool set );
void X11_windowChangeProperty( Display *display, Window window, const char *property, const char *value );
void X11_windowSetDecoration( Display *display, Window window, bool set );



#endif
