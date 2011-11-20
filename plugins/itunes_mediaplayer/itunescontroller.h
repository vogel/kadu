#ifndef ITUNESCONTROLLER_H
#define ITUNESCONTROLLER_H

#include <CoreFoundation/CoreFoundation.h>
#include <QtCore/QString>

#include "tune.h"


class ITunesController
{
public:
	ITunesController();
	~ITunesController();

	Tune currentTune();

private:
	static void iTunesCallback(CFNotificationCenterRef, void*, CFStringRef, const void*, CFDictionaryRef info);
	static void iTunesPlaylistCallback(CFNotificationCenterRef,void* observer,CFStringRef,const void*, CFDictionaryRef info);
	Tune currentTune_;
};

#endif
