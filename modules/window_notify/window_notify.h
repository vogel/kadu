#ifndef WINDOW_NOTIFY_H
#define WINDOW_NOTIFY_H
#include <qobject.h>
#include <qstring.h>

#include "gadu.h"

class WindowNotify : QObject
{
	Q_OBJECT
	public:
		WindowNotify();
		~WindowNotify();
	
	public slots:
		void newChat(UinsList senders, const QString& msg, time_t time, bool &grab);
		void newMessage(UinsList senders, const QString& msg, time_t time);
		void connectionError(const QString &message);
		void userChangingStatus(const UinType uin, const unsigned int oldstatus, const unsigned int status);
		void userChangedStatusToAvailable(const UserListElement &ule);
		void userChangedStatusToBusy(const UserListElement &ule);
		void userChangedStatusToNotAvailable(const UserListElement &ule);
		void message(const QString &from, const QString &type, const QString &message, const UserListElement *ule);
};

extern WindowNotify *window_notify;

#endif
