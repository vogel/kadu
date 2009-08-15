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


// VERSION: 1.01.00


/*
KNOWN ISSUES:
- Compiz does not return window decoration width and height, however
  it includes it when moving window.
- SetCurrentDesktop() won't work for vertical desktop switching when
  the Compiz's Cube is active (this seems to be a Cube bug)
*/


#ifndef __X11TOOLS_H
	#define __X11TOOLS_H


#include <stdint.h>
#include <string>
#include <utility>
#include <X11/Xlib.h>


#define  X11_ALLDESKTOPS                0xFFFFFFFF  /*long*/
#define  X11_BADDESKTOP                 0xFFFFFFFE  /*long*/

#define  X11_SETACTIVEWINDOW_TIMEOUT     (50*1000)  /*usec*/
#define  X11_SETACTIVEWINDOW_CHECKTIME    (2*1000)  /*usec*/

#define  MWM_HINTS_DECORATIONS           (1L << 1)


typedef struct
{
	uint32_t flags;
	uint32_t functions;
	uint32_t decorations;
	 int32_t input_mode;
	uint32_t status;
} MotifWMHints;


bool X11_getCardinalProperty( Display *display, Window window, const char *propertyName, long *value, int offset = 0 );
bool X11_getAtomProperty( Display *display, Window window, const char *propertyName, Atom *value );

std::pair<int,int> X11_getResolution( Display *display );
std::pair<int,int> X11_getDesktopSize( Display *display );

std::pair<int,int> X11_getMousePos( Display *display );

bool X11_isFreeDesktopCompatible( Display *display );

long X11_getDesktopsCount( Display *display, bool forceFreeDesktop = false );
long X11_getCurrentDesktop( Display *display, bool forceFreeDesktop = false );
void X11_setCurrentDesktop( Display *display, long desktop, bool forceFreeDesktop = false );
long X11_getDesktopOfWindow( Display *display, Window window, bool forceFreeDesktop = false, bool windowareadecides = true );
void X11_moveWindowToDesktop( Display *display, Window window, long desktop, bool forceFreeDesktop = false, bool position = false, int x = 0, int y = 0 );
bool X11_isWindowVisibleOnDesktop( Display *display, Window window, long desktop, bool forceFreeDesktop = false );
bool X11_isWholeWindowOnOneDesktop( Display *display, Window window );
bool X11_isWindowFullyVisible( Display *display, Window window );

std::pair<int,int> X11_getWindowPos( Display *display, Window window );
std::pair<int,int> X11_getWindowSize( Display *display, Window window );
std::pair<int,int> X11_getWindowFramelessSize( Display *display, Window window );
void X11_moveWindow( Display *display, Window window, int x, int y );
void X11_centerWindow( Display *display, Window window, long desktop = X11_BADDESKTOP, bool forceFreeDesktop = false );
void X11_resizeWindow( Display *display, Window window, int width, int height );

Window X11_getActiveWindow( Display *display );
void X11_setActiveWindow( Display *display, Window window, bool forceFreeDesktop = false );

Window X11_getWindowUnderCursor( Display *display, int *rootx = NULL, int *rooty = NULL, int *windowx = NULL, int *windowy = NULL );
Window X11_getInnerMostWindowUnderCursor( Display *display, int *rootx = NULL, int *rooty = NULL, int *windowx = NULL, int *windowy = NULL );

std::string X11_getWindowClass( Display *display, Window window );
std::string X11_getWindowRole( Display *display, Window window );

void X11_windowSendXEvent( Display *display, Window window, const char *type, const char *message, bool set );
void X11_windowChangeProperty( Display *display, Window window, const char *property, const char *value );
void X11_windowSetDecoration( Display *display, Window window, bool set );


#endif
