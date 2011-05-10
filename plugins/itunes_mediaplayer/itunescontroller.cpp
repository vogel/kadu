/*
 * itunescontroller.cpp
 * Copyright (C) 2006  Remko Troncon
 * Modified by Tomasz "Dorregaray" Rostanski
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/*
 * Sample track information:
 *	<dict>
 *		<key>Track ID</key><integer>36</integer>
 *		<key>Name</key><string>The Stars of Track and Field</string>
 *		<key>Artist</key><string>Belle & Sebastian</string>
 *		<key>Album</key><string>If You're Feeling Sinister</string>
 *		<key>Genre</key><string>Rock/Pop</string>
 *		<key>Kind</key><string>MPEG audio file</string>
 *		<key>Size</key><integer>4621294</integer>
 *		<key>Total Time</key><integer>288182</integer>
 *		<key>Track Number</key><integer>1</integer>
 *		<key>Date Modified</key><date>2004-06-21T11:20:55Z</date>
 *		<key>Date Added</key><date>2004-07-30T01:47:28Z</date>
 *		<key>Bit Rate</key><integer>128</integer>
 *		<key>Sample Rate</key><integer>44100</integer>
 *		<key>Play Count</key><integer>1</integer>
 *		<key>Play Date</key><integer>-1119524786</integer>
 *		<key>Play Date UTC</key><date>2004-08-15T23:21:50Z</date>
 *		<key>Artwork Count</key><integer>1</integer>
 *		<key>Track Type</key><string>File</string>
 *		<key>Location</key><string>file://localhost/Users/zakariya/Music/iTunes/iTunes%20Music/Belle%20&%20Sebastian/If%20You're%20Feeling%20Sinister/01%20The%20Stars%20of%20Track%20and%20Field.mp3</string>
 *		<key>File Folder Count</key><integer>4</integer>
 *		<key>Library Folder Count</key><integer>1</integer>
 *	</dict>
 */

#include <QtGlobal>
#include <QtCore/QString>
#include <QtCore/QTime>
#include <QtCore/QDateTime>
#include <QtCore/QScopedArrayPointer>

#include <CoreFoundation/CoreFoundation.h>

#include "itunescontroller.h"

/**
 * \class ITunesController
 * \brief A controller for the Mac OS X version of iTunes.
 */

static QString CFStringToQString(CFStringRef s)
{
	QString result;

	if (s != NULL)
	{
		CFIndex length = CFStringGetMaximumSizeForEncoding(CFStringGetLength(s), kCFStringEncodingUTF8) + 1;
		QScopedArrayPointer<char> buffer(new char[length]);
		if (CFStringGetCString(s, buffer.data(), length, kCFStringEncodingUTF8))
			result = QString::fromUtf8(buffer.data());
		else
			qWarning("itunesplayer.cpp: CFString conversion failed.");
	}
	return result;
}


ITunesController::ITunesController()
{
	// TODO: Poll iTunes for current playing tune
	CFNotificationCenterRef center = CFNotificationCenterGetDistributedCenter();
	CFNotificationCenterAddObserver(center, this, ITunesController::iTunesCallback, CFSTR("com.apple.iTunes.playerInfo"), NULL, CFNotificationSuspensionBehaviorDeliverImmediately);
}

ITunesController::~ITunesController()
{
	CFNotificationCenterRef center = CFNotificationCenterGetDistributedCenter();
	CFNotificationCenterRemoveObserver(center, this, CFSTR("com.apple.iTunes.playerInfo"), NULL);
}

Tune ITunesController::currentTune()
{
	return currentTune_;
}

void ITunesController::iTunesCallback(CFNotificationCenterRef,void* observer,CFStringRef,const void*, CFDictionaryRef info)
{
	Tune tune;
	ITunesController* controller = (ITunesController*) observer;

	CFStringRef cf_state = (CFStringRef) CFDictionaryGetValue(info, CFSTR("Player State"));
	if (CFStringCompare(cf_state,CFSTR("Paused"), 0) == kCFCompareEqualTo)
	{
		tune.setState(Tune::paused);
	}
	else if (CFStringCompare(cf_state,CFSTR("Stopped"), 0) == kCFCompareEqualTo)
	{
		tune.setState(Tune::stopped);
	}
	else if (CFStringCompare(cf_state,CFSTR("Playing"), 0) == kCFCompareEqualTo)
	{
		tune.setState(Tune::playing);

		QString name = CFStringToQString((CFStringRef) CFDictionaryGetValue(info, CFSTR("Name")));
		if (name != tune.name())
		{
			tune.setStarted(QDateTime::currentDateTime().toTime_t());
		}

		tune.setArtist(CFStringToQString((CFStringRef) CFDictionaryGetValue(info, CFSTR("Artist"))));
		tune.setName(name);
		tune.setAlbum(CFStringToQString((CFStringRef) CFDictionaryGetValue(info, CFSTR("Album"))));
		tune.setLocation(CFStringToQString((CFStringRef) CFDictionaryGetValue(info, CFSTR("Location"))));

		CFNumberRef cf_track = (CFNumberRef) CFDictionaryGetValue(info, CFSTR("Track Number"));
		int track = 0;
		if (cf_track)
		{
			if (!CFNumberGetValue(cf_track, kCFNumberIntType, &track))
			{
				qWarning("itunesplayer.cpp: Number value conversion failed.");
			}
		}
		tune.setTrack(track);

		CFNumberRef cf_time = (CFNumberRef) CFDictionaryGetValue(info, CFSTR("Total Time"));
		int time = 0;
		if (cf_time)
		{
			if (!CFNumberGetValue(cf_time,kCFNumberIntType,&time))
			{
				qWarning("itunesplayer.cpp: Number value conversion failed.");
			}
		}
		tune.setTime(time);
	}
	else
	{
		tune.setState(Tune::unknown);
		qWarning("itunesplayer.cpp: Unknown state.");
	}
	controller->currentTune_ = tune;
}
