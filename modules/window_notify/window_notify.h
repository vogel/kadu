#ifndef WINDOW_NOTIFY_H
#define WINDOW_NOTIFY_H
#include <qobject.h>
#include <qstring.h>

#include "gadu.h"

class WindowNotify : public QObject
{
	Q_OBJECT
	public:
		WindowNotify(QObject *parent=0, const char *name=0);
		~WindowNotify();

	public slots:
		void newChat(const UinsList &senders, const QString& msg, time_t time);
		void newMessage(const UinsList &senders, const QString& msg, time_t time, bool &grab);
		void connectionError(const QString &message);
		void userStatusChanged(const UserListElement &ule, const UserStatus &oldStatus);
		void userChangedStatusToAvailable(const UserListElement &ule);
		void userChangedStatusToBusy(const UserListElement &ule);
		void userChangedStatusToNotAvailable(const UserListElement &ule);
		void message(const QString &from, const QString &message, const QMap<QString, QVariant> *parameters, const UserListElement *ule);
};

extern WindowNotify *window_notify;

#endif
