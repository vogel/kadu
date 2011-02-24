/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <algorithm>

#include <QtCore/QRect>
#include <QtGui/QBitmap>
#include <QtGui/QPainter>
#include <QtGui/QPixmap>
#include <QtGui/QX11Info>

#include "pixmap-grabber.h"

#define MINIMUM_SIZE 8

//////////////////////////////////////////////////////////////////
// Code below is copied (and changed a little) from KSnapShot,
// copyrighted by Bernd Brandstetter <bbrand@freenet.de>.
// It's licenced with GPL.

static bool operator < (const QRect &r1, const QRect &r2)
{
    return r1.width() * r1.height() < r2.width() * r2.height();
}

QPixmap PixmapGrabber::grabCurrent()
{
    Window root;
    int y, x;
    uint w, h, border, depth;
    XGrabServer( QX11Info::display() );
    Window child = windowUnderCursor( /*includeDecorations*/true );
    XGetGeometry( QX11Info::display(), child, &root, &x, &y, &w, &h, &border, &depth );
    Window parent;
    Window* children;
    unsigned int nchildren;
    if( XQueryTree( QX11Info::display(), child, &root, &parent,
                    &children, &nchildren ) != 0 ) {
        if( children != NULL )
            XFree( children );
        int newx, newy;
        Window dummy;
        if( XTranslateCoordinates( QX11Info::display(), parent, QX11Info::appRootWindow(),
            x, y, &newx, &newy, &dummy )) {
            x = newx;
            y = newy;
        }
    }
    QPixmap pm( grabWindow( child, x, y, w, h, border ) );
    XUngrabServer( QX11Info::display() );
    return pm;
}

Window PixmapGrabber::windowUnderCursor(bool includeDecorations)
{
    Window root;
    Window child;
    uint mask;
    int rootX, rootY, winX, winY;
    XGrabServer( QX11Info::display() );
    XQueryPointer( QX11Info::display(), QX11Info::appRootWindow(), &root, &child,
                   &rootX, &rootY, &winX, &winY, &mask );
    if( child == None )
        child = QX11Info::appRootWindow();
    if( !includeDecorations ) {
        Window real_child = findRealWindow( child );
        if( real_child != None ) // test just in case
            child = real_child;
    }
    return child;
}

QPixmap PixmapGrabber::grabWindow( Window child, int x, int y, uint w, uint h, uint border )
{
    QPixmap pm( QPixmap::grabWindow( QX11Info::appRootWindow(), x, y, w, h ) );

    int tmp1, tmp2;
    //Check whether the extension is available
    if ( XShapeQueryExtension( QX11Info::display(), &tmp1, &tmp2 ) ) {
        QBitmap mask( w, h );
        //As the first step, get the mask from XShape.
        int count, order;
        XRectangle* rects = XShapeGetRectangles( QX11Info::display(), child,
                                                 ShapeBounding, &count, &order );
        //The ShapeBounding region is the outermost shape of the window;
        //ShapeBounding - ShapeClipping is defined to be the border.
        //Since the border area is part of the window, we use bounding
        // to limit our work region
        if (rects) {
            //Create a QRegion from the rectangles describing the bounding mask.
            QRegion contents;
            for ( int pos = 0; pos < count; pos++ )
                contents += QRegion( rects[pos].x, rects[pos].y,
                                     rects[pos].width, rects[pos].height );
            XFree( rects );

            //Create the bounding box.
            QRegion bbox( 0, 0, w, h );

            if( border > 0 ) {
                contents.translate( border, border );
                contents += QRegion( 0, 0, border, h );
                contents += QRegion( 0, 0, w, border );
                contents += QRegion( 0, h - border, w, border );
                contents += QRegion( w - border, 0, border, h );
            }

            //Get the masked away area.
            QRegion maskedAway = bbox - contents;
            QVector<QRect> maskedAwayRects = maskedAway.rects();

            //Construct a bitmap mask from the rectangles
            QPainter p(&mask);
            p.fillRect(0, 0, w, h, Qt::color1);
            for (int pos = 0; pos < maskedAwayRects.count(); pos++)
                    p.fillRect(maskedAwayRects[pos], Qt::color0);
            p.end();

            pm.setMask(mask);
        }
    }

    return pm;
}

// Recursively iterates over the window w and its children, thereby building
// a tree of window descriptors. Windows in non-viewable state or with height
// or width smaller than minSize will be ignored.
void PixmapGrabber::getWindowsRecursive( std::vector<QRect>& windows, Window w, int rx, int ry, int depth )
{
    XWindowAttributes atts;
    XGetWindowAttributes( QX11Info::display(), w, &atts );
    if ( atts.map_state == IsViewable &&
         atts.width >= MINIMUM_SIZE && atts.height >= MINIMUM_SIZE ) {
        int x = 0, y = 0;
        if ( depth ) {
            x = atts.x + rx;
            y = atts.y + ry;
        }

        QRect r( x, y, atts.width, atts.height );
        if ( std::find( windows.begin(), windows.end(), r ) == windows.end() ) {
            windows.push_back( r );
        }

        Window root, parent;
        Window* children;
        unsigned int nchildren;

        if( XQueryTree( QX11Info::display(), w, &root, &parent, &children, &nchildren ) != 0 ) {
            for( unsigned int i = 0; i < nchildren; ++i ) {
                getWindowsRecursive( windows, children[ i ], x, y, depth + 1 );
            }
            if( children != NULL )
                XFree( children );
        }
    }
    if ( depth == 0 )
        std::sort( windows.begin(), windows.end() );
}

Window PixmapGrabber::findRealWindow( Window w, int depth )
{
    if( depth > 5 )
        return None;
    static Atom wm_state = XInternAtom( QX11Info::display(), "WM_STATE", False );
    Atom type;
    int format;
    unsigned long nitems, after;
    unsigned char* prop;
    if( XGetWindowProperty( QX11Info::display(), w, wm_state, 0, 0, False, AnyPropertyType,
        &type, &format, &nitems, &after, &prop ) == Success ) {
        if( prop != NULL )
            XFree( prop );
        if( type != None )
            return w;
    }
    Window root, parent;
    Window* children;
    unsigned int nchildren;
    Window ret = None;
    if( XQueryTree( QX11Info::display(), w, &root, &parent, &children, &nchildren ) != 0 ) {
        for( unsigned int i = 0;
             i < nchildren && ret == None;
             ++i )
            ret = findRealWindow( children[ i ], depth + 1 );
        if( children != NULL )
            XFree( children );
    }
    return ret;
}

// End of code copied from KSnapShot
//////////////////////////////////////////////////////////////////
