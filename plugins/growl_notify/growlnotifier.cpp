/*
 * growlnotifier.cpp - A simple Qt interface to Growl
 *
 * Copyright (C) 2005  Remko Troncon
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * You can also redistribute and/or modify this program under the
 * terms of the Psi License, specified in the accompanied COPYING
 * file, as published by the Psi Project; either dated January 1st,
 * 2005, or (at your option) any later version.
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

/**
 * \class GrowlNotifier
 * \todo Write a destructor, which CFReleases all datastructures
 */

extern "C" {
#include <CoreFoundation/CoreFoundation.h>
#include <Growl/Growl.h>
}

#include <QtCore/QBuffer>
#include <QtCore/QDir>
#include <QtCore/QScopedArrayPointer>
#include <QtCore/QStringList>
#include <QtGui/QPixmap>

#include "notification/notification/notification.h"

#include "growlnotifier.h"

static bool IsNotifying = false;

/**
 * \brief Converts a QString to a CoreFoundation string, preserving Unicode.
 *
 * \param s the string to be converted.
 * \return a reference to a CoreFoundation string.
 */
static CFStringRef QString2CFString(const QString& s)
{
	if (s.isNull())
		return 0;

	QScopedArrayPointer<ushort> buffer(new ushort[s.length()]);
	for (int i = 0; i < s.length(); ++i)
		buffer[i] = s[i].unicode();

	CFStringRef result = CFStringCreateWithBytes(NULL,
			(UInt8 *)buffer.data(), s.length() * sizeof(ushort),
			kCFStringEncodingUnicode, false);

	return result;
}


/**
 * \brief Retrieves the values from the context.
 *
 * \param context the context
 * \param receiver the receiving object which will be signaled when the
 *	notification is clicked. May be NULL.
 * \param clicked_slot the slot to be signaled when the notification is clicked.
 * \param timeout_slot the slot to be signaled when the notification isn't clicked.
 * \param context the context which will be passed back to the slot
 *	May be NULL.
 */
void getContext( CFPropertyListRef context, GrowlNotifierSignaler** signaler, const QObject** receiver, const char** clicked_slot, const char** timeout_slot, void** qcontext/*, pid_t* pid*/)
{
	CFDataRef data;

	if (signaler) {
		data = (CFDataRef) CFArrayGetValueAtIndex((CFArrayRef) context, 0);
		CFDataGetBytes(data, CFRangeMake(0,CFDataGetLength(data)), (UInt8*) signaler);
	}

	if (receiver) {
		data = (CFDataRef) CFArrayGetValueAtIndex((CFArrayRef) context, 1);
		CFDataGetBytes(data, CFRangeMake(0,CFDataGetLength(data)), (UInt8*) receiver);
	}

	if (clicked_slot) {
		data = (CFDataRef) CFArrayGetValueAtIndex((CFArrayRef) context, 2);
		CFDataGetBytes(data, CFRangeMake(0,CFDataGetLength(data)), (UInt8*) clicked_slot);
	}

	if (timeout_slot) {
		data = (CFDataRef) CFArrayGetValueAtIndex((CFArrayRef) context, 3);
		CFDataGetBytes(data, CFRangeMake(0,CFDataGetLength(data)), (UInt8*) timeout_slot);
	}

	if (qcontext) {
		data = (CFDataRef) CFArrayGetValueAtIndex((CFArrayRef) context, 4);
		CFDataGetBytes(data, CFRangeMake(0,CFDataGetLength(data)), (UInt8*) qcontext);
	}
}


/**
 * Creates a context for a notification, which will be sent back by Growl
 * when a notification is clicked.
 *
 * \param receiver the receiving object which will be signaled when the
 *	notification is clicked. May be NULL.
 * \param clicked_slot the slot to be signaled when the notification is clicked.
 * \param timeout_slot the slot to be signaled when the notification isn't clicked.
 * \param context the context which will be passed back to the slot
 *	May be NULL.
 * \return the context
 */
CFPropertyListRef createContext( GrowlNotifierSignaler* signaler, const QObject* receiver, const char* clicked_slot, const char* timeout_slot, void* qcontext /*, pid_t pid*/)
{
	CFDataRef context[5];

	context[0] = CFDataCreate(kCFAllocatorDefault, (const UInt8*) &signaler, sizeof(GrowlNotifierSignaler*));
	context[1] = CFDataCreate(kCFAllocatorDefault, (const UInt8*) &receiver, sizeof(const QObject *));
	context[2] = CFDataCreate(kCFAllocatorDefault, (const UInt8*) &clicked_slot, sizeof(const char*));
	context[3] = CFDataCreate(kCFAllocatorDefault, (const UInt8*) &timeout_slot, sizeof(const char*));
	context[4] = CFDataCreate(kCFAllocatorDefault, (const UInt8*) &qcontext, sizeof(void*));

	CFArrayRef array = CFArrayCreate( kCFAllocatorDefault,
                (const void **)context, 5, &kCFTypeArrayCallBacks );

	// Cleaning up
	CFRelease(context[0]);
	CFRelease(context[1]);
	CFRelease(context[2]);
	CFRelease(context[3]);
	CFRelease(context[4]);

	return array;
}


/**
 * The callback function, used by Growl to notify that a notification was
 * clicked.
 * \param context the context of the notification
 */
void notification_clicked(CFPropertyListRef context)
{
	GrowlNotifierSignaler* signaler;
	const QObject* receiver;
	const char* slot;
	void* qcontext;

	getContext(context, &signaler, &receiver, &slot, 0, &qcontext/*, &pid*/);

	QObject::connect(signaler, SIGNAL(notificationClicked(void*)), receiver, slot);
	signaler->emitNotificationClicked(qcontext);
	QObject::disconnect(signaler, SIGNAL(notificationClicked(void*)), receiver, slot);

	Notification *notification = (Notification *)receiver;
	notification->release();
	IsNotifying = false;
}


/**
 * The callback function, used by Growl to notify that a notification has
 * timed out.
 * \param context the context of the notification
 */
void notification_timeout(CFPropertyListRef context)
{
	GrowlNotifierSignaler* signaler;
	const QObject* receiver;
	const char* slot;
	void* qcontext;

	getContext(context, &signaler, &receiver, 0, &slot, &qcontext /*, &pid*/);
	if (slot != NULL)
	{
		QObject::connect(signaler, SIGNAL(notificationTimedOut(void*)), receiver, slot);
		signaler->emitNotificationTimeout(qcontext);
		QObject::disconnect(signaler, SIGNAL(notificationTimedOut(void*)), receiver, slot);
	}
	Notification *notification = (Notification *)receiver;
	notification->release();
	IsNotifying = false;
}


/**
 * Returns whether growl is installed or not.
 * Used in growl_notify_init
 */
bool grow_is_installed()
{
	return Growl_IsInstalled();
}

/**
 * Constructs a GrowlNotifier.
 *
 * \param notifications the list names of all notifications that can be sent
 *	by this notifier.
 * \param default_notifications the list of names of the notifications that
 *  should be enabled by default.
 * \param app the name of the application under which the notifier should
 *	register with growl.
 */
GrowlNotifier::GrowlNotifier(
	const QStringList& notifications, const QStringList& default_notifications,
	const QString& app)
{
	CFStringRef notification;

	// Initialize signaler
	signaler_ = new GrowlNotifierSignaler();

	// All Notifications
	QStringList::ConstIterator it;
	CFMutableArrayRef allNotifications = CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks);
	for (it = notifications.begin(); it != notifications.end(); ++it)
	{
		notification = QString2CFString(*it);
		CFArrayAppendValue(allNotifications, notification);
		CFRelease(notification);
	}

	// Default Notifications
	CFMutableArrayRef defaultNotifications = CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks);
	for (it = default_notifications.begin(); it != default_notifications.end(); ++it)
	{
		notification = QString2CFString(*it);
		CFArrayAppendValue(defaultNotifications, notification);
		CFRelease(notification);
	}

	// Initialize delegate
	InitGrowlDelegate(&delegate_);
	delegate_.applicationName = QString2CFString(app);

	CFTypeRef keys[] = { GROWL_NOTIFICATIONS_ALL, GROWL_NOTIFICATIONS_DEFAULT };
	CFTypeRef values[] = { allNotifications, defaultNotifications };

	delegate_.registrationDictionary = CFDictionaryCreate(
		kCFAllocatorDefault, keys, values, 2,
		&kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);

	delegate_.growlNotificationWasClicked = &notification_clicked;
	delegate_.growlNotificationTimedOut = &notification_timeout;

	CFRelease(allNotifications);
	CFRelease(defaultNotifications);

	// Register with Growl
	Growl_SetDelegate(&delegate_);
}

GrowlNotifier::~GrowlNotifier()
{
	delete signaler_;
}

/**
 * \brief Sends a notification to Growl.
 *
 * \param name the registered name of the notification.
 * \param title the title for the notification.
 * \param description the description of the notification.
 * \param icon the icon of the notification.
 * \param sticky whether the notification should be sticky (i.e. require a
 *	click to discard.
 * \param receiver the receiving object which will be signaled when the
 *	notification is clicked. May be NULL.
 * \param slot the slot to be signaled when the notification is clicked.
 * \param context the context which will be passed back to the slot
 *	May be NULL.
 */
void GrowlNotifier::notify(const QString& name, const QString& title,
	const QString& description, const QPixmap& p, bool sticky,
	const QObject* receiver,
	const char* clicked_slot, const char* timeout_slot,
	void* qcontext)
{
	// Convert the image if necessary
	CFDataRef icon = 0;
	if (!p.isNull()) {
		QByteArray img_data;
		QBuffer buffer(&img_data);
		buffer.open(QIODevice::WriteOnly);
		p.save(&buffer, "PNG");
		icon = CFDataCreate( NULL, (const UInt8 *) img_data.constData(), img_data.size());
	}

	// Convert strings
	CFStringRef cf_title = QString2CFString(title);
	CFStringRef cf_description = QString2CFString(description);
	CFStringRef cf_name = QString2CFString(name);

	// Do notification
	CFPropertyListRef context = createContext(signaler_, receiver, clicked_slot, timeout_slot, qcontext/*, getpid()*/);
	Growl_NotifyWithTitleDescriptionNameIconPriorityStickyClickContext(
		cf_title, cf_description, cf_name, icon, 0, sticky, context);
	IsNotifying = true;

	// Release intermediary datastructures
	CFRelease(context);
	if (icon)
		CFRelease(icon);
	if (cf_title)
		CFRelease(cf_title);
	if (cf_description)
		CFRelease(cf_description);
	if (cf_name)
		CFRelease(cf_name);
}

bool GrowlNotifier::isNotifying()
{
	return IsNotifying;
};

static void removeTemps(const QString &path)
{
	QDir d = QDir(path);
	QStringList filter;
	filter << "*.growlRegDict";
	QStringList files = d.entryList(filter, QDir::Files, QDir::NoSort);
	foreach (const QString &file, files)
		d.remove(file);
}

static void checkDirectory(const QString &path)
{
	QDir d = QDir(path);
	QStringList filter;
	filter << "TemporaryItems";
	QStringList subdirs = d.entryList(filter, QDir::Dirs, QDir::NoSort);
	foreach (const QString &subdir, subdirs)
		removeTemps(path + '/' + subdir);
}

void GrowlNotifier::cleanupAfterGrowl()
{
	/* As Growl is not doing this for us (as it should) we have to do it ourself
	 * This is ugly but I see no other way to do that. Hope it will work for
	 * everyone */
	QDir d = QDir("/private/var/folders/VM");
	QStringList filter;
	filter << "VM*";
	QStringList subdirs = d.entryList(filter, QDir::Dirs, QDir::NoSort);
	foreach (const QString &subdir, subdirs)
		checkDirectory(d.absolutePath() + '/' + subdir);
}

#include "moc_growlnotifier.cpp"
