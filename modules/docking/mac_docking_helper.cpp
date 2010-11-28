/*
 * %kadu copyright begin%
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
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

#include <QtGui/QApplication>
#include <QtGui/QIcon>

#include "ApplicationServices/ApplicationServices.h"
#include "Carbon/Carbon.h"

#include "mac_docking_helper.h"

#define DOCK_FONT_NAME "LucidaGrande-Bold"
#define DOCK_FONT_SIZE 24

MacDockingHelper *MacDockingHelper::Instance = 0;

MacDockingHelper::MacDockingHelper(QObject *parent) : QObject(parent)
{
	isBouncing = false;
	isOverlayed = false;
}

MacDockingHelper::~MacDockingHelper()
{
	stopBounce();
	removeOverlay();
}

void MacDockingHelper::startBounce()
{
	/* The following code is taken from PSI mac_dock sources */
	if (!isBouncing)
	{
		bounceRec.qType = nmType;
		bounceRec.nmMark = 1;
		bounceRec.nmIcon = NULL;
		bounceRec.nmSound = NULL;
		bounceRec.nmStr = NULL;
		bounceRec.nmResp = NULL;
		bounceRec.nmRefCon = 0;
		NMInstall(&bounceRec);
		isBouncing = true;
	}
}

void MacDockingHelper::stopBounce()
{
	/* The following code is taken from PSI mac_dock sources */
	if (isBouncing)
	{
		NMRemove(&bounceRec);
		isBouncing = false;
	}
}

void MacDockingHelper::removeOverlay()
{
	if (isOverlayed)
	{
		isOverlayed = false;

		CGContextRef context = BeginCGContextForApplicationDockTile();
		CGContextRestoreGState(context);
		CGContextFlush(context);
		EndCGContextForApplicationDockTile(context);

		qApp->setWindowIcon(qApp->windowIcon());
		//RestoreApplicationDockTileImage();
	}
}

void MacDockingHelper::overlay(const QString& text)
{
	/* The following code is taken from PSI mac_dock sources */

	CGContextRef context = BeginCGContextForApplicationDockTile();

	if (!isOverlayed)
	{
		CGContextSaveGState(context);
		isOverlayed = true;

		// Add some subtle drop down shadow
		CGSize s = { 2.0, -4.0 };
		CGContextSetShadow(context, s, 5.0);
	}

	// Draw a circle
	CGContextBeginPath(context);
	CGContextAddArc(context, 95.0, 95.0, 25.0, 0.0, 2 * M_PI, true);
	CGContextClosePath(context);
	CGContextSetRGBFillColor(context, 1, 0.0, 0.0, 1);
	CGContextFillPath(context);

	// Set the clipping path to the same circle
	CGContextBeginPath(context);
	CGContextAddArc(context, 95.0, 95.0, 25.0, 0.0, 2 * M_PI, true);
	CGContextClip(context);

	// Select the appropriate font
	CGContextSelectFont(context,DOCK_FONT_NAME, DOCK_FONT_SIZE, kCGEncodingMacRoman);
	CGContextSetRGBFillColor(context, 1, 1, 1, 1);

	// Draw the text invisible
	CGPoint begin = CGContextGetTextPosition(context);
	CGContextSetTextDrawingMode(context, kCGTextInvisible);
	CGContextShowTextAtPoint(context, begin.x, begin.y, text.toStdString().c_str(), text.length());
	CGPoint end = CGContextGetTextPosition(context);

	// Draw the text
	CGContextSetTextDrawingMode(context, kCGTextFill);
	CGContextShowTextAtPoint(context, 95 - (end.x - begin.x)/2, 95 - 8, text.toStdString().c_str(), text.length());

	// Cleanup
	CGContextFlush(context);
	EndCGContextForApplicationDockTile(context);
}

