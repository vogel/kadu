/*
 * %kadu copyright begin%
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010, 2011 Tomasz Rostanski (rozteck@interia.pl)
 * %kadu copyright end%
 *
 * Copyright 2011 Adam "Vertex" Makświej (vertexbz@gmail.com)
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
#include <Cocoa/Cocoa.h>

#include "ApplicationServices/ApplicationServices.h"
#include "mac_docking_helper.h"
#include "core/core.h"
#include "gui/windows/kadu-window.h"
#include "configuration/configuration-file.h"
#include "docking.h"

@interface MacDockingHelperObjC : NSObject {
}

- (id) init;
- (void) appReopen:(NSAppleEventDescriptor *)event withReplyEvent:(NSAppleEventDescriptor *)replyEvent;
@end

@implementation MacDockingHelperObjC
- (id) init
{
	NSAutoreleasePool *pool = [NSAutoreleasePool new];
	[super init];
	[[NSAppleEventManager sharedAppleEventManager] setEventHandler:self
		andSelector:@selector(appReopen:withReplyEvent:)
		forEventClass:kCoreEventClass
		andEventID:kAEReopenApplication];
	[pool release];
	return self;
}

- (void) dealloc
{
	NSAutoreleasePool *pool = [NSAutoreleasePool new];
	[[NSAppleEventManager sharedAppleEventManager] removeEventHandlerForEventClass:kCoreEventClass
		andEventID:kAEReopenApplication];
	[pool release];
	[super dealloc];
}

- (void) appReopen:(NSAppleEventDescriptor *)event withReplyEvent:(NSAppleEventDescriptor *)replyEvent
{
	Q_UNUSED(event)
	Q_UNUSED(replyEvent)
	DockingManager::instance()->dockIconClicked();
}
@end

struct MacDockingHelperStruct { MacDockingHelperObjC * macDockingHelperObjC; };

MacDockingHelper *MacDockingHelper::Instance = 0;

MacDockingHelper::MacDockingHelper(QObject *parent) : QObject(parent) , d( new MacDockingHelperStruct )
{
	isBouncing = false;
	d->macDockingHelperObjC = [[MacDockingHelperObjC alloc] init];
	if (config_file.readBoolEntry("General", "RunDocked"))
		Core::instance()->setShowMainWindowOnStart(false);
	Core::instance()->kaduWindow()->setDocked(true);
}

MacDockingHelper::~MacDockingHelper()
{
	stopBounce();
	removeOverlay();
	if (!Core::instance()->isClosing())
		Core::instance()->kaduWindow()->show();
	Core::instance()->kaduWindow()->setDocked(false);
	[d->macDockingHelperObjC release];
	delete d;
}

void MacDockingHelper::startBounce()
{
	if (!isBouncing)
	{
		currentAttentionRequest = [NSApp requestUserAttention:NSCriticalRequest];
		isBouncing = true;
	}
}

void MacDockingHelper::stopBounce()
{
	if (isBouncing)
	{
		[NSApp cancelUserAttentionRequest:currentAttentionRequest];
		isBouncing = false;
	}
}

void MacDockingHelper::removeOverlay()
{
	[[[NSApplication sharedApplication] dockTile]setBadgeLabel:nil];
	qApp->setWindowIcon(qApp->windowIcon());
}

void MacDockingHelper::overlay(const NSInteger count)
{
	if (count == 0) {
		removeOverlay();
		return;
	}
	QPixmap pixmap = qApp->windowIcon().pixmap(128, 128);
	CGImageRef image = pixmap.toMacCGImageRef();

	NSRect imageRect = NSMakeRect(0.0, 0.0, CGImageGetWidth(image), CGImageGetHeight(image));
	NSImage *newImage = 0;
	newImage = [[NSImage alloc] initWithSize:imageRect.size];
	[newImage lockFocus];
	{
		CGContextRef imageContext = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
		CGContextDrawImage(imageContext, *(CGRect *)&imageRect, image);
	}
	[newImage unlockFocus];
	CFRelease(image);

	[NSApp setApplicationIconImage:newImage];
	[newImage release];
	[[[NSApplication sharedApplication] dockTile]setBadgeLabel:[NSString stringWithFormat:@"%d", count]];
}
